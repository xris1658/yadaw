import QtQuick
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    title: qsTr("Add Track")
    color: Colors.background
    flags: Qt.Dialog
    width: tabContent.width + 10
    height: column.height + 10
    Column {
        id: column
        anchors.centerIn: parent
        spacing: 10
        TabBar {
            id: trackTypeTabBar
            TabButton {
                id: audioTrackButton
                width: implicitWidth
                text: qsTr("Audio")
            }
            TabButton {
                id: instrumentButton
                width: implicitWidth
                text: qsTr("Instrument")
            }
            TabButton {
                id: midiButton
                width: implicitWidth
                text: "MIDI"
            }
            TabButton {
                id: audioEffectButton
                width: implicitWidth
                text: qsTr("Audio Effect")
            }
        }
        Item {
            id: tabContent
            width: firstColumnWidth + secondColumnWidth + columnSpacing
            height: stackLayout.height
            property int firstColumnWidth: 100
            property int secondColumnWidth: 150
            property int columnSpacing: 10
            property int rowSpacing: 5
            StackLayout {
                id: stackLayout
                anchors.centerIn: parent
                currentIndex: trackTypeTabBar.currentIndex
                Grid {
                    columns: 2
                    columnSpacing: tabContent.columnSpacing
                    rowSpacing: tabContent.rowSpacing
                    verticalItemAlignment: Grid.AlignVCenter
                    horizontalItemAlignment: Grid.AlignHCenter
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Name") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    TextField {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Channel Config") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Input") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Output") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Count") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    SpinBox {
                        width: tabContent.secondColumnWidth
                        height: implicitHeight
                        editable: true
                        from: 1
                    }
                }
                Grid {
                    columns: 2
                    columnSpacing: tabContent.columnSpacing
                    rowSpacing: tabContent.rowSpacing
                    verticalItemAlignment: Grid.AlignVCenter
                    horizontalItemAlignment: Grid.AlignHCenter
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Name") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    TextField {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Instrument") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Input") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Output") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Count") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    SpinBox {
                        width: tabContent.secondColumnWidth
                        height: implicitHeight
                        editable: true
                        from: 1
                    }
                }
                Grid {
                    columns: 2
                    columnSpacing: tabContent.columnSpacing
                    rowSpacing: tabContent.rowSpacing
                    verticalItemAlignment: Grid.AlignVCenter
                    horizontalItemAlignment: Grid.AlignHCenter
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Name") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    TextField {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Input") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Output") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Count") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    SpinBox {
                        width: tabContent.secondColumnWidth
                        height: implicitHeight
                        editable: true
                        from: 1
                    }
                }
                Grid {
                    columns: 2
                    columnSpacing: tabContent.columnSpacing
                    rowSpacing: tabContent.rowSpacing
                    verticalItemAlignment: Grid.AlignVCenter
                    horizontalItemAlignment: Grid.AlignHCenter
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Name") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    TextField {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Channel Config") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Audio Effect") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Output") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    ComboBox {
                        width: tabContent.secondColumnWidth
                    }
                    Label {
                        width: tabContent.firstColumnWidth
                        text: qsTr("Count") + ":"
                        horizontalAlignment: Label.AlignRight
                    }
                    SpinBox {
                        width: tabContent.secondColumnWidth
                        height: implicitHeight
                        editable: true
                        from: 1
                    }
                }
            }
        }
        Item {
            width: tabContent.width
            height: buttons.height
            Row {
                id: buttons
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: okButton
                    text: qsTr("&Add")
                    onClicked: {
                        // TODO
                        root.close();
                    }
                }
                Button {
                    id: cancelButton
                    text: Constants.cancelTextWithMnemonic
                    onClicked: {
                        root.close();
                    }
                }
            }
        }
    }
}
