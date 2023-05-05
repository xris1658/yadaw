import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.controlBorder
    property int minimumWidth: 500
    property int minimumHeight: 500
    property alias model: busList.model

    Column {
        Item {
            width: busList.width
            height: addBusButton.height
            Label {
                height: addBusButton.height + addBusButton.topPadding + addBusButton.bottomPadding
                verticalAlignment: Label.AlignVCenter
                text: busList.count.toString() + (busList.count <= 1? qsTr(" audio bus"): qsTr(" audio buses"))
                leftPadding: addBusButton.rightPadding
                Component.onCompleted: {
                    console.log(busList.count);
                }
            }
            Button {
                id: addBusButton
                anchors.right: parent.right
                anchors.rightMargin: rightPadding
                anchors.top: parent.top
                anchors.topMargin: topPadding
                text: qsTr("&Add Bus...")
            }
        }
        ListView {
            id: busList
            width: root.width / 2
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
    }

    Row {
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
