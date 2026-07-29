#include "audio/plugin/PluginText.hpp"
#include "base/Constants.hpp"
#include "controller/ConfigController.hpp"
#include "controller/LocalizationController.hpp"
#include "event/EventBase.hpp"
#include "event/EventHandler.hpp"
#include "event/SplashScreenWorkerThread.hpp"
#include "entity/EntityInitializer.hpp"
#include "model/ModelInitializer.hpp"
#include "native/CPU.hpp"
#include "native/Native.hpp"
#include "ui/MessageDialog.hpp"
#include "ui/Runtime.hpp"
#include "ui/UI.hpp"

#if _WIN32
#include <QQuickGraphicsConfiguration>
#endif

#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>

int main(int argc, char *argv[])
{
    YADAW::Native::fillCPUIDInfo();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    YADAW::UI::qmlApplicationEngine = &engine;
    YADAW::Entity::initializeEntity();
    YADAW::Model::initializeModel();
#if _WIN32
    YADAW::UI::D3DFlipSwitcher* d3dFlipSwitcher = nullptr;
    // [*] Force enable PreMulAlpha on D3D11 swap chain so that
    //     DirectComposition is used.
    //     This is the 3rd of a 3-part workaround of `QQuickWindow` with D3D11
    //     RHI failing [the smooth resize test by Raph Levien](https://raphlinus.github.io/rust/gui/2019/06/21/smooth-resize-test.html).
    //     This workaround is almost blatantly copied from [xi-editor/xi-win #21 also by Raph Levien](https://github.com/xi-editor/xi-win/pull/21).
    //     See part 1 in src/ui/win/D3DFlipSwitcher.hpp and part 2 in tools/d3d11-rhi-changes
    //     TODO: Refactor the modified D3D11 support into a plugin since
    //           not everyone wants to build Qt on thier own.
    if(auto quickBackend = std::getenv("QT_QUICK_BACKEND");
        !quickBackend || std::strcmp(quickBackend, "rhi") == 0)
    {
        if(auto rhiBackend = std::getenv("QSG_RHI_BACKEND");
            !rhiBackend || std::strcmp(rhiBackend, "d3d11") == 0)
        {
            auto defaultFormat = QSurfaceFormat::defaultFormat();
            if(defaultFormat.alphaBufferSize() <= 0)
            {
                defaultFormat.setAlphaBufferSize(8);
                QSurfaceFormat::setDefaultFormat(defaultFormat);
                d3dFlipSwitcher = &YADAW::UI::d3dFlipSwitcher();
                app.installNativeEventFilter(d3dFlipSwitcher);
            }
        }
    }
#endif
    const QString eventsName("Events.qml");
    const QString splashScreenName("SplashScreen.qml");
    QObject* splashScreen = nullptr;
    const QString mainWindowName("YADAW.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app,
        [&](QObject *obj, const QUrl &objUrl)
        {
            if(!obj)
            {
#if NDEBUG
#else
                fprintf(stderr, "Press <ENTER> to exit...");
                getchar();
#endif
                std::exit(-1);
            }
            const auto& fileName = objUrl.fileName();
            if(fileName == eventsName)
            {
                YADAW::Event::eventSender = obj->property("eventSender").value<QObject*>();
                YADAW::Event::eventReceiver = obj->property("eventReceiver").value<QObject*>();
            }
            else if(fileName == splashScreenName)
            {
                splashScreen = obj;
            }
            else if(fileName == mainWindowName)
            {
                YADAW::UI::mainWindow = qobject_cast<QQuickWindow*>(obj);
#if _WIN32
                auto& d3dFlipSwitcher = YADAW::UI::d3dFlipSwitcher();
                d3dFlipSwitcher.addWindow(*YADAW::UI::mainWindow);
                auto gc = YADAW::UI::mainWindow->graphicsConfiguration();
                gc.setDebugLayer(true);
                YADAW::UI::mainWindow->setGraphicsConfiguration(gc);
#endif
            }
        },
        Qt::DirectConnection
    );
    YADAW::Controller::initializeApplicationConfig();
    auto config = YADAW::Controller::loadConfig();
    // Qt on Windows uses DirectWrite as the default font engine since
    // Qt 6.8, while older versions of Qt uses GDI. DirectWrite supports
    // some advanced font features like variable fonts, but YADAW does not
    // use those features for now (maybe it is necessary while working on
    // localizations with variable fonts, but I'm not very sure).
    // I initially added this feature to let MacType improve the font
    // rendering, but my configuration of MacType does not handle
    // DirectWrite (since text in other applications that use DirectWrite
    // like Settings will get blurry if DirectWrite support is enabled),
    // making the text looks unsmooth and not antialiased.
    // TODO: Might have to change this option and let user select font
    //       backends if needed?
    // If you are looking for some temporary solution since you encountered
    // this issue, you can either append
    // -platform windows:fontengine=gdi
    // to the program argument, or add a file called qt.conf with content:
    //
    // [Platforms]
    // WindowsArguments = fontengine=gdi
    //
    // to the application directory.
    auto systemFontRendering = config["general"]["system-font-rendering"].as<bool>();
    if(systemFontRendering)
    {
        if((!YADAW::Native::isDebuggerPresent()) || config["general"]["system-font-rendering-while-debugging"].as<bool>())
        {
            QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
        }
    }
    QDir dir(YADAW::UI::defaultFontDir());
    if(dir.exists())
    {
        const auto& entryInfoList = dir.entryInfoList(QDir::Filter::Files | QDir::Filter::Hidden);
        for(const auto& entryInfo: entryInfoList)
        {
            QFontDatabase::addApplicationFont(entryInfo.absoluteFilePath());
        }
    }
    auto language = QString::fromStdString(config["general"]["language"].as<std::string>());
    auto& localizationList = YADAW::Controller::appLocalizationListModel();
    std::vector<QTranslator*> translators;
    for(int i = 0; i < localizationList.itemCount(); ++i)
    {
        const auto& localization = localizationList.at(i);
        if(localization.name == language)
        {
            const auto& translationFileList = localization.translationFileList;
            translators.reserve(translationFileList.size());
            decltype(translationFileList.size()) fileCountLoaded = std::count_if(
                translationFileList.begin(), translationFileList.end(),
                [&](const QString& translationFile)
                {
                    auto& pTranslator = translators.emplace_back();
                    pTranslator = new(std::nothrow) QTranslator(&app);
                    if(pTranslator)
                    {
                        return pTranslator->load(QLocale(localization.languageCode), translationFile);
                    }
                    return false;
                }
            );
            for(auto pTranslator: translators)
            {
                QCoreApplication::installTranslator(pTranslator);
            }
            for(const auto& font: localization.fontList)
            {
                QFontDatabase::addApplicationFont(font);
            }
            QString fontName("Fira Sans");
            for(const auto& fontFamily: localization.fontFamilyList)
            {
                QFont::insertSubstitution(fontName, fontFamily);
            }
            if(fileCountLoaded != translationFileList.size())
            {
                YADAW::UI::createMessageDialog();
                YADAW::UI::setHideCloseButton(true);
                auto messageDialog = YADAW::UI::messageDialog;
                if(messageDialog)
                {
                    messageDialog->setProperty(
                        "icon",
                        QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
                    );
                    messageDialog->setProperty(
                        "message",
                        QVariant::fromValue<QString>(
                            "Failed to load some translation files. YADAW will use the files loaded, with English as fallback."
                        )
                    );
                    messageDialog->setTitle(YADAW::Base::ProductName);
                    messageDialog->setModality(Qt::WindowModality::ApplicationModal);
                }
                YADAW::UI::focusMessageDialogButton(0);
                messageDialog->showNormal();
                YADAW::UI::getMessageDialogResult();
            }
            YADAW::Controller::currentTranslationIndex = i;
            break;
        }
    }
    if(YADAW::Controller::currentTranslationIndex == -1)
    {
        YADAW::UI::createMessageDialog();
        YADAW::UI::setHideCloseButton(true);
        auto messageDialog = YADAW::UI::messageDialog;
        if(messageDialog)
        {
            messageDialog->setProperty(
                "icon",
                QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
            );
            messageDialog->setProperty(
                "message",
                QVariant::fromValue<QString>("The selecting translation file does not exist. YADAW is showing English as fallback.")
            );
            messageDialog->setTitle(YADAW::Base::ProductName);
            messageDialog->setModality(Qt::WindowModality::ApplicationModal);
            messageDialog->setVisible(true);
            YADAW::UI::focusMessageDialogButton(0);
            messageDialog->showNormal();
            YADAW::UI::getMessageDialogResult();
        }
        YADAW::Controller::currentTranslationIndex = 0;
    }
#if 0
    if(YADAW::Native::isDebuggerPresent())
    {
        YADAW::UI::createMessageDialog();
        YADAW::UI::setHideCloseButton(true);
        if(auto messageDialog = YADAW::UI::messageDialog)
        {
            messageDialog->setProperty(
                "icon",
                QVariant::fromValue<int>(YADAW::UI::IconType::Warning)
            );
            messageDialog->setProperty(
                "message",
                QVariant::fromValue<QString>(
                    YADAW::Audio::Plugin::getDebugModeWarningText()
                )
            );
            messageDialog->setTitle(
                YADAW::Audio::Plugin::getDebugModeWarningTitle()
            );
            messageDialog->setModality(Qt::WindowModality::ApplicationModal);
            messageDialog->setVisible(true);
            qDebug("focusMessageDialogButton");
            YADAW::UI::focusMessageDialogButton(0);
            messageDialog->showNormal();
            YADAW::UI::getMessageDialogResult();
        }
    }
#endif
    auto& timer = YADAW::UI::idleProcessTimer();
    engine.loadFromModule("Main", "Events");
    YADAW::Event::EventHandler eh(YADAW::Event::eventSender, YADAW::Event::eventReceiver);
    YADAW::Event::eventHandler = &eh;
    engine.loadFromModule("content", "SplashScreen");
    YADAW::Event::SplashScreenWorkerThread sswt(splashScreen);
    YADAW::Event::splashScreenWorkerThread = &sswt;
    QObject::connect(&sswt, &YADAW::Event::SplashScreenWorkerThread::openMainWindow,
        &eh, &YADAW::Event::EventHandler::onOpenMainWindow,
        Qt::ConnectionType::QueuedConnection);
    sswt.start();
    // engine.load(url);
    // eventHandler.setQtVersion(QString(qVersion()));
    auto ret = app.exec();
#if _WIN32
    if(d3dFlipSwitcher)
    {
        app.removeNativeEventFilter(d3dFlipSwitcher);
    }
#endif
    return ret;
}
