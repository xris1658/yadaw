import QtQml
import QtQuick

Rectangle {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth
    Grid {
        columns: 2
        columnSpacing: 10
        rowSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        Label {
            width: firstColumnWidth
            text: qsTr("I/O")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Input Device")
            horizontalAlignment: Label.AlignRight
        }
        ComboBox {
            width: secondColumnWidth
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Output Device")
            horizontalAlignment: Label.AlignRight
        }
        ComboBox {
            width: secondColumnWidth
        }
    }
}