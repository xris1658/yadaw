qt_add_library(content STATIC)

set_source_files_properties(Colors.qml
                            Constants.qml
                            EventReceiver.qml
                            EventSender.qml
                            Functions.qml
                            Global.qml
                            Mnemonic.qml
                            PROPERTIES
                            QT_QML_SINGLETON_TYPE true
                            )

set(QT_QML_IMPORT_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/imports CACHE STRING "" FORCE)

set(CONTENT_CPP_FILES
    model/IAssetDirectoryListModel.hpp
    model/IAudioBusChannelListModel.hpp
    model/IAudioBusConfigurationModel.hpp
    model/IAudioGraphDeviceListModel.hpp
    model/IAutomationModel.hpp
    model/ILocalizationListModel.hpp
    model/ModelBase.hpp
    model/IPluginDirectoryListModel.hpp
    model/IPluginListModel.hpp
    )

set(CONTENT_QML_FILES
    AboutWindow.qml
    AboutQtWindow.qml
    Action.qml
    ApplicationWindow.qml
    Assets.qml
    AudioBusConfiguration.qml
    AudioBusConfigurationWindow.qml
    AudioGraphSettings.qml
    AudioHardwareSettings.qml
    AudioIcon.qml
    Automation.qml
    BigClock.qml
    Button.qml
    BypassIcon.qml
    CLAPIcon.qml
    CheckBox.qml
    ColorPicker.qml
    ColorPickerWindow.qml
    Colors.qml
    ComboBox.qml
    Constants.qml
    Detail.qml
    EventReceiver.qml
    EventSender.qml
    FaderIcon.qml
    FolderIcon.qml
    FollowIcon.qml
    Functions.qml
    GeneralSettings.qml
    Global.qml
    GroupIcon.qml
    IOIcon.qml
    InsertIcon.qml
    ItemDelegate.qml
    Label.qml
    LoopIcon.qml
    MainWindow.qml
    Menu.qml
    MenuBar.qml
    MenuBarItem.qml
    MenuItem.qml
    MenuSeparator.qml
    MessageDialog.qml
    MetronomeIcon.qml
    MIDIEffectIcon.qml
    Mixer.qml
    MixerChannel.qml
    Mnemonic.qml
    PianoKeysIcon.qml
    PlayIcon.qml
    PluginIcon.qml
    PluginListView.qml
    PluginSettings.qml
    PreferencesWindow.qml
    ProgressBar.qml
    RemoveIcon.qml
    RenameIcon.qml
    ScrollBar.qml
    SendIcon.qml
    ShowLeftPaneIcon.qml
    Slider.qml
    SortIcon.qml
    SplashScreen.qml
    SplitView.qml
    Switch.qml
    TabBar.qml
    TabButton.qml
    TextField.qml
    ThirdPartySoftware.qml
    TimeIndicator.qml
    VestifalIcon.qml
    VST3Icon.qml
    WindowIcon.qml
    )

message(NOTICE "Close the UI project in Qt Design Studio BEFORE building things. Otherwise fonts cannot be copied.")

qt_add_qml_module(content
                  URI "content"
                  VERSION 1.0
                  QML_FILES
                    ${CONTENT_QML_FILES}
                  SOURCES
                    ${CONTENT_CPP_FILES}
                  RESOURCES
                    qmldir
                    fonts/FiraSans-Bold.otf
                    fonts/FiraSans-Regular.otf
                    fonts/FiraSansCondensed-Bold.otf
                    fonts/FiraSansCondensed-Regular.otf
                    images/check.png
                    images/error.png
                    images/info.png
                    images/question.png
                    images/warning.png
                    images/Splashscreen.png
                    images/Main-icon.ico
                    images/qtlogo-64.png)

qt_add_translations(content TS_FILES ../l10n/osct/YADAW_zh_CN.ts
                    INCLUDE_DIRECTORIES ./content
                    SOURCES ${CONTENT_QML_FILES})