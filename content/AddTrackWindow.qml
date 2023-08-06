import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("Add Track")
    modality: Qt.WindowModal
    color: Colors.background

    width: tabContent.width + 10
    height: column.height + 10

    property alias trackType: stackLayout.currentIndex

    enum TrackType {
        Audio,
        Instrument,
        MIDI,
        AudioEffect
    }

    Column {
        id: column
        anchors.centerIn: parent
        spacing: 10
        TabBar {
            id: trackTypeTabBar
            currentIndex: root.trackType
            TabButton {
                id: audioTrackButton
                width: implicitWidth
                text: qsTr("Audio")
                onClicked: {
                    root.trackType = TabBar.index;
                }
            }
            TabButton {
                id: instrumentButton
                width: implicitWidth
                text: qsTr("Instrument")
                onClicked: {
                    root.trackType = TabBar.index;
                }
            }
            TabButton {
                id: midiButton
                width: implicitWidth
                text: "MIDI"
                onClicked: {
                    root.trackType = TabBar.index;
                }
            }
            TabButton {
                id: audioEffectButton
                width: implicitWidth
                text: qsTr("Audio Effect")
                onClicked: {
                    root.trackType = TabBar.index;
                }
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
                        id: audioNameField
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
                    StackLayout.onIsCurrentItemChanged: {
                        if(StackLayout.isCurrentItem) {
                            audioNameField.forceActiveFocus();
                        }
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
                        id: instrumentNameField
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
                    StackLayout.onIsCurrentItemChanged: {
                        if(StackLayout.isCurrentItem) {
                            instrumentNameField.forceActiveFocus();
                        }
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
                        id: midiNameField
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
                    StackLayout.onIsCurrentItemChanged: {
                        if(StackLayout.isCurrentItem) {
                            midiNameField.forceActiveFocus();
                        }
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
                        id: audioEffectNameField
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
                    StackLayout.onIsCurrentItemChanged: {
                        if(StackLayout.isCurrentItem) {
                            audioEffectNameField.forceActiveFocus();
                        }
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
        Keys.onTabPressed: (event) => {
            if(event.modifiers & Qt.ControlModifier) {
                if(event.modifiers & Qt.ShiftModifier) {
                    let index = trackType === 0? stackLayout.count - 1: trackType - 1;
                    root.trackType = index;
                }
                else {
                    let index = trackType === stackLayout.count - 1? 0: trackType + 1;
                    root.trackType = index;
                }
            }
        }
    }
}
