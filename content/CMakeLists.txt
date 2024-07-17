qt_add_library(content STATIC)

# If Qt 6.5.3 is used, then the application crashes while a Singleton object is
# sending a signal. See https://bugreports.qt.io/browse/QTBUG-117852
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
    entity/AudioBackendSupport.hpp
    entity/ChannelConfig.hpp
    entity/IAudioIOPosition.hpp
    entity/PluginFormatSupport.hpp
    model/FilterRoleModel.hpp
    model/IAssetDirectoryListModel.hpp
    model/IAudioBusChannelListModel.hpp
    model/IAudioBusConfigurationModel.hpp
    model/IAudioDeviceIOGroupListModel.hpp
    model/IAudioDeviceListModel.hpp
    model/IAudioIOPositionModel.hpp
    model/IAutomationModel.hpp
    model/IFileTreeModel.hpp
    model/ILocalizationListModel.hpp
    model/IMixerChannelInsertListModel.hpp
    model/IMixerChannelListModel.hpp
    model/MessageDialogButtonBoxModel.hpp
    model/ModelBase.hpp
    model/IParameterValueAndTextListModel.hpp
    model/IPluginCategoryListModel.hpp
    model/IPluginDirectoryListModel.hpp
    model/IPluginListModel.hpp
    model/IPluginParameterListModel.hpp
    model/ISendListModel.hpp
    model/ISortFilterListModel.hpp
    model/ISpeakerListModel.hpp
    model/ITrackListModel.hpp
    model/SortFilterProxyListModel.hpp
    model/SortOrderModel.hpp
    )

set(CONTENT_QML_FILES
    AboutWindow.qml
    AboutQtWindow.qml
    Action.qml
    AddTrackWindow.qml
    ALSASettings.qml
    ApplicationWindow.qml
    Arrangement.qml
    Assets.qml
    AudioBusConfiguration.qml
    AudioBusConfigurationWindow.qml
    AudioFXIcon.qml
    AudioGraphSettings.qml
    AudioHardwareSettings.qml
    AudioIcon.qml
    AudioIOIcon.qml
    AudioIOSelector.qml
    Automation.qml
    AuxInputIcon.qml
    AuxOutputIcon.qml
    BigClock.qml
    Button.qml
    BypassIcon.qml
    CLAPIcon.qml
    CheckBox.qml
    ColorPicker.qml
    ColorPickerWindow.qml
    Colors.qml
    ComboBox.qml
    ComboBoxButton.qml
    Constants.qml
    Detail.qml
    Dial.qml
    DialogButtons.qml
    EventReceiver.qml
    EventSender.qml
    FaderIcon.qml
    FolderIcon.qml
    FollowIcon.qml
    Functions.qml
    GeneralSettings.qml
    GenericPluginEditor.qml
    Global.qml
    GroupIcon.qml
    HeadphoneIcon.qml
    IOIcon.qml
    InsertIcon.qml
    ItemDelegate.qml
    Label.qml
    LocateIcon.qml
    LoopIcon.qml
    MainWindow.qml
    Menu.qml
    MenuBar.qml
    MenuBarItem.qml
    MenuItem.qml
    MenuSeparator.qml
    MessageDialog.qml
    MetronomeIcon.qml
    MIDIEditor.qml
    MIDIEffectIcon.qml
    MIDIIOIcon.qml
    Mixer.qml
    MixerChannel.qml
    MixerInsertSlot.qml
    Mnemonic.qml
    MnemonicFunctions.js
    PianoKeys.qml
    PianoKeysIcon.qml
    PlayIcon.qml
    PluginIcon.qml
    PluginRouteEditor.qml
    PluginScanProgressWindow.qml
    PluginSelector.qml
    PluginSettings.qml
    PluginWindow.qml
    PreferencesWindow.qml
    ProgressBar.qml
    ProgressBarLikeSlider.qml
    RemoveIcon.qml
    RenameIcon.qml
    RouteList.qml
    ScrollBar.qml
    SendButton.qml
    SendIcon.qml
    ShowLeftPaneIcon.qml
    Slider.qml
    SortIcon.qml
    SpinBox.qml
    SplashScreen.qml
    SplitView.qml
    Switch.qml
    TabBar.qml
    TabButton.qml
    TableLikeListView.qml
    TableLikeTreeView.qml
    TagIcon.qml
    TapTempoWindow.qml
    TextField.qml
    ThirdPartySoftware.qml
    TimeIndicator.qml
    Timeline.qml
    ToolTip.qml
    TrackHeader.qml
    VestifalIcon.qml
    VST3Icon.qml
    WASAPIExclusiveSettings.qml
    WindowIcon.qml
    )

message(NOTICE "Close the UI project in Qt Design Studio BEFORE building things. Otherwise fonts cannot be copied.")

qt_add_qml_module(content
                  URI "content"
                  VERSION 1.0
                  NO_PLUGIN
                  QML_FILES
                    ${CONTENT_QML_FILES}
                  SOURCES
                    ${CONTENT_CPP_FILES}
                  RESOURCES
                    qmldir
                    fonts/FiraSans-Bold.otf
                    fonts/FiraSans-BoldItalic.otf
                    fonts/FiraSansCondensed-Bold.otf
                    fonts/FiraSansCondensed-BoldItalic.otf
                    fonts/FiraSansCondensed-Italic.otf
                    fonts/FiraSansCondensed-Regular.otf
                    fonts/FiraSans-Italic.otf
                    fonts/FiraSans-Regular.otf
                    images/check.png
                    images/error.png
                    images/info.png
                    images/question.png
                    images/warning.png
                    images/Splashscreen.png
                    images/Main-icon.ico
                    images/qtlogo-64.png)

set_source_files_properties(../l10n/osct/YADAW_content_zh_CN.ts
                            DIRECTORY ..
                            PROPERTIES
                            OUTPUT_LOCATION
                            ${CMAKE_CURRENT_BINARY_DIR}/../l10n/osct)

qt_add_translations(content TS_FILES ../l10n/osct/YADAW_content_zh_CN.ts
                    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
                    SOURCES ${CONTENT_QML_FILES})