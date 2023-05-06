import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts

Rectangle {
    id: root
    color: Colors.background
    property int minimumWidth: 400
    property int minimumHeight: 300
    property alias model: busList.model

    Column {
        Item {
            id: busListHeader
            width: busList.width
            height: addBusButton.height + addBusButton.topPadding + addBusButton.bottomPadding
            Label {
                height: addBusButton.height + addBusButton.topPadding + addBusButton.bottomPadding
                verticalAlignment: Label.AlignVCenter
                text: busList.count.toString() + (busList.count <= 1? qsTr(" audio bus"): qsTr(" audio buses"))
                leftPadding: addBusButton.rightPadding
            }
            Button {
                id: addBusButton
                anchors.right: parent.right
                anchors.rightMargin: rightPadding
                anchors.top: parent.top
                anchors.topMargin: topPadding
                text: qsTr("&Add Bus...")
                QC.Popup {
                    id: popup
                    x: (parent.width - width) / 2
                    y: parent.height + parent.bottomPadding
                    width: popupRow.width + popupRow.spacing * 2
                    height: popupRow.height + popupRow.spacing * 2
                    background: Rectangle {
                        color: Colors.background
                        border.width: 1
                        border.color: Colors.controlBorder
                    }
                    Row {
                        id: popupRow
                        anchors.centerIn: parent
                        spacing: 3
                        TextField {
                            id: channelCountTextField
                            width: height * 5
                            placeholderText: qsTr("Channel count")
                            onAccepted: {
                                okAddBusButton.clicked();
                            }
                        }
                        Button {
                            id: okAddBusButton
                            text: Constants.okTextWithMnemonic
                            onClicked: {
                                root.model.append(parseInt(channelCountTextField.text));
                                popup.close();
                            }
                        }
                    }
                    onOpened: {
                        channelCountTextField.clear();
                        channelCountTextField.forceActiveFocus();
                    }
                }
                onClicked: {
                    popup.open();
                }
            }
        }
        ListView {
            id: busList
            width: root.width / 2
            height: root.height - busListHeader.height
            clip: true
            ScrollBar.vertical: ScrollBar {
                id: busListScrollBar
                visible: busList.height < busList.contentHeight
            }
            delegate: ItemDelegate {
                width: busList.width - (busListScrollBar.visible? busListScrollBar.width: 0)
                text: abcm_name == ""? qsTr("Bus ") + (index + 1): abcm_name
            }
        }
    }
    StackLayout {
        id: stackLayout
        anchors.right: parent.right
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
