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

#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTranslator>
#include <QVersionNumber>

int main(int argc, char *argv[])
{
    YADAW::Native::fillCPUIDInfo();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    YADAW::UI::qmlApplicationEngine = &engine;
    YADAW::Entity::initializeEntity();
    YADAW::Model::initializeModel();
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
            }
        },
        Qt::DirectConnection
    );
#if _WIN32
    // A temporary workaround to solve/avoid GUI performance issues on Windows
    // while dragging the window.
    // On Windows, Direct3D 11 is the default backend of the scene graph
    // threaded renderer. When the D3D11 swap chain is created in flip mode
    // (i.e. with `DXGI_SWAP_EFFECT_FLIP_xxx` as swap effect) with 2 buffers
    // (hard coded as `QD3D11SwapChain::BUFFER_COUNT`), swapping frames
    // takes too much time (50ms at most per swap operation under 60Hz, which is
    // about 3 frames) when dragging a window, causing visible stutters (you can
    // see it in other Qt applications that uses Direct3D 11, like this one:
    // https://github.com/giladreich/QtDirect3D/issues/10).
    // By tinkering QQuickRenderControl D3D11 Example, I found some solutions to
    // this (that works on my machine and might not be optimal to others):
    // - Create the swap chain in bitblt mode and discard back buffers (with
    //   swap effect `DXGI_SWAP_EFFECT_DISCARD`), and leave the buffer count (2)
    //   as is. This can be done by setting the environment variable
    //   `QT_D3D_NO_FLIP`, which is added BACK* in Qt 6.6.1. Since it's
    //   undocumented, it might not work in future versions of Qt.
    //   Note that bitblt swap chain cannot be created if the flag contains
    //   `DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING`, which is used if vsync is disabled
    //   (by setting `QSG_NO_VSYNC` to 1).
    //   (This variable was removed in Qt 6.5.0: https://github.com/qt/qtbase/commit/3ee65daa74f36434b5a3d78b509247511d7867e1
    //   And this commit added it back: https://github.com/qt/qtbase/commit/bd76a9a86fbca0b646c59b9a2796d63cdac8373c
    //   )
    if(QLibraryInfo::version() >= QVersionNumber(6, 6, 1))
    {
        qputenv("QT_D3D_NO_FLIP", "1");
    }
    // - Create the swap chain with `DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL` as the
    //   swap effect, use 3 buffers and leave the mode as is.
    //   (This method is used in Zed for Windows: https://github.com/zed-industries/zed/blob/main/crates/gpui/src/platform/windows/directx_renderer.rs
    //   but it is removed in Qt 6.5.0: https://github.com/qt/qtbase/commit/17b542c2d2545924f4d36f6705352e5b2489aa10
    //   )
    //   Seems like it takes too long to discard the back buffers in flip mode.
    // - Create the swap chain with 5 buffers, and leave others as is.
    // As for older Qt versions before Qt 6.6.1, I just avoid using Direct3D by
    // switching to software rendering.
    // TODO: Control the rendering process by using `QQuickRenderControl`
    // I don't use OpenGL or Vulkan as the backend because visible artifacts
    // (black screen, Windows 7 classic title bar) occur for about 1s while
    // entering or leaving full screen mode, which is even LONGER than software
    // rendering.
    else
    {
        qputenv("QSG_NO_VSYNC", "1");
        QQuickWindow::setGraphicsApi(QSGRendererInterface::GraphicsApi::Software);
    }
#else
    qputenv("QSG_NO_VSYNC", "1");
#endif
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
            qDebug("focusMessageDialogButton");
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
    return ret;
}
