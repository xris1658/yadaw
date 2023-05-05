import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.controlBorder
    property int minimumWidth: 300
    property int minimumHeight: 300
    property alias model: busList.model

    Row {
        ListView {
            id: busList
            width: root.minimumWidth / 2
            height: root.height
            ScrollBar.vertical: ScrollBar {
                id: busListScrollBar
                visible: busList.height < busList.contentHeight
            }
            delegate: ItemDelegate {
                width: busList.width - (busListScrollBar.visible? busListScrollBar.width: 0)
                text: abcm_name
            }
        }
        StackLayout {
            id: stackLayout
            width: root.minimumWidth - busList.width
            height: root.height
            Repeater {
                model: busList.model
                ListView {
                    id: channelList
                    width: stackLayout.width
                    model: abcm_channel_list
                    ScrollBar.vertical: ScrollBar {
                        id: channelListScrollBar
                        visible: channelList.height < channelList.contentHeight
                    }

                    delegate: ItemDelegate {
                        text: "Device " + (abclm_device_index + 1).toString() + ", Channel " + (abclm_channel_index + 1).toString()
                        width: busList.width - (channelListScrollBar.visible? channelListScrollBar.width: 0)
                    }
                }
            }
        }
    }
}
