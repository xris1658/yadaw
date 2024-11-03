import QtQuick
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    title: qsTr("Preferences")
    color: Colors.background
    flags: Qt.Dialog
    modality: Qt.NonModal

    QtObject {
        id: impl
        property int firstColumnWidth: 125
        property int secondColumnWidth: stackLayout.width - firstColumnWidth - stackLayout.anchors.rightMargin
    }

    property alias systemFontRendering: generalSettings.systemFontRendering
    property alias systemFontRenderingWhileDebugging: generalSettings.systemFontRenderingWhileDebugging
    property alias translationModel: generalSettings.translationModel
    property alias currentTranslationIndex: generalSettings.currentTranslationIndex

    property alias currentAudioBackend: audioHardwareSettings.currentBackend
    property alias audioInputBusConfigurationModel: audioHardwareSettings.audioInputBusConfigurationModel
    property alias audioOutputBusConfigurationModel: audioHardwareSettings.audioOutputBusConfigurationModel

    property alias audioInputDeviceList: audioHardwareSettings.audioInputDeviceList
    property alias audioOutputDeviceList: audioHardwareSettings.audioOutputDeviceList

    property alias audioGraphInputDeviceList: audioHardwareSettings.audioGraphInputDeviceList
    property alias audioGraphOutputDeviceList: audioHardwareSettings.audioGraphOutputDeviceList
    property alias audioGraphOutputDeviceIndex: audioHardwareSettings.audioGraphOutputDeviceIndex
    property alias audioGraphBufferSize: audioHardwareSettings.audioGraphBufferSize
    property alias audioGraphLatency: audioHardwareSettings.audioGraphLatency
    property alias alsaInputDeviceList: audioHardwareSettings.alsaInputDeviceList
    property alias alsaOutputDeviceList: audioHardwareSettings.alsaOutputDeviceList

    property alias pluginDirectoryListModel: pluginSettings.directoryListModel

    signal audioGraphOutputDeviceChanged(deviceIndex: int)
    signal startPluginScan()
    signal pluginScanComplete()
    onPluginScanComplete: {
        pluginSettings.scanPluginComplete();
    }

    width: 540
    height: 500
    minimumWidth: 540
    SplitView {
        id: content
        anchors.fill: parent
        orientation: Qt.Horizontal
        handle: Item {
            implicitWidth: 5
            implicitHeight: 5
            Rectangle {
                width: 1
                height: parent.height
                anchors.right: parent.right
                color: Colors.controlBorder
            }
        }
        ListView {
            topMargin: 5
            bottomMargin: 5
            leftMargin: 5
            id: listView
            SplitView.preferredWidth: 110
            model: [qsTr("General"), qsTr("Audio Hardware"), qsTr("Plugins")]
            boundsBehavior: ListView.StopAtBounds
            delegate: ItemDelegate {
                id: itemDelegate
                width: parent.width
                // height: implicitHeight
                text: modelData
                leftPadding: 2
                rightPadding: 2
                topPadding: 2
                bottomPadding: 2
                highlighted: index == listView.currentIndex
                background: Rectangle {
                    width: itemDelegate.width
                    height: itemDelegate.height
                    anchors.topMargin: root.topInset
                    anchors.bottomMargin: root.bottomInset
                    anchors.leftMargin: root.leftInset
                    anchors.rightMargin: root.rightInset
                    color: (!itemDelegate.enabled)? Colors.background:
                        itemDelegate.highlighted? Colors.highlightControlBackground:
                            itemDelegate.hovered? Colors.mouseOverControlBackground: Colors.background
                }
                onClicked: {
                    listView.currentIndex = index;
                }
            }
        }
        Item {
            StackLayout {
                id: stackLayout
                anchors.fill: parent
                anchors.margins: 10
                currentIndex: listView.currentIndex
                GeneralSettings {
                    id: generalSettings
                    firstColumnWidth: impl.firstColumnWidth
                    secondColumnWidth: impl.secondColumnWidth
                }
                AudioHardwareSettings {
                    id: audioHardwareSettings
                    firstColumnWidth: impl.firstColumnWidth
                    secondColumnWidth: impl.secondColumnWidth
                }
                PluginSettings {
                    id: pluginSettings
                    firstColumnWidth: impl.firstColumnWidth
                    secondColumnWidth: impl.secondColumnWidth
                    onStartPluginScan: {
                        root.startPluginScan();
                    }
                }
            }
        }
        Keys.onEscapePressed: {
            close();
        }
    }
    Component.onCompleted: {
        content.forceActiveFocus();
    }
}
