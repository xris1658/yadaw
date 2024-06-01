import QtQuick
import QtQuick.Controls as QC

QC.Popup {
    id: root

    signal accepted()
    signal cancelled()
    signal resetted()

    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    padding: 10

    QtObject {
        id: impl
        property int contentWidth: 300
    }

    background: Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        color: Colors.background
        border.color: Colors.border
    }

    contentItem: Column {
        spacing: 5
        TextField {
            id: searchTextField
            placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
            width: impl.contentWidth
        }
        Rectangle {
            width: impl.contentWidth
            height: 300
            color: "transparent"
            border.color: Colors.border
            SplitView {
                anchors.fill: parent
                anchors.margins: parent.border.width
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
                Item {
                    id: categoryList
                    property int currentIndex: 0
                    SplitView.preferredWidth: 100
                    Column {
                        topPadding: 5
                        bottomPadding: 5
                        spacing: 5
                        ListView {
                            width: parent.width
                            height: contentHeight
                            model: ListModel {
                                ListElement {
                                    name: qsTr("Audio Bus")
                                }
                                ListElement {
                                    name: qsTr("Audio Group Channel")
                                }
                                ListElement {
                                    name: qsTr("Audio Effect Channel")
                                }
                                ListElement {
                                    name: qsTr("Plugin Aux Input")
                                }
                                ListElement {
                                    name: qsTr("Plugin Aux Output")
                                }
                            }
                            delegate: ItemDelegate {
                                width: parent.width - 3 * 2
                                height: implicitHeight
                                text: name
                                leftPadding: height
                                rightPadding: 2
                                topPadding: 2
                                bottomPadding: 2
                            }
                        }
                    }
                }
                TableLikeListView {
                    //
                }
            }
        }
        Item {
            width: impl.contentWidth
            height: cancelButton.height
            Button {
                id: resetButton
                text: qsTr("R&eset")
                onClicked: {
                    root.resetted();
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: okButton
                    text: Constants.okTextWithMnemonic
                    onClicked: {
                        root.accepted();
                    }
                }
                Button {
                    id: cancelButton
                    text: Constants.cancelTextWithMnemonic
                    onClicked: {
                        root.cancelled();
                    }
                }
            }
        }
    }
}
