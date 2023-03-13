import QtQml
import QtQuick

Item {
    id: root
    width: firstColumnWidth + secondColumnWidth + grid.columnSpacing * 3

    property alias systemFontRendering: systemFontRenderingSwitch.checked
    property int firstColumnWidth
    property int secondColumnWidth

    Grid {
        id: grid
        columns: 2
        columnSpacing: 10
        rowSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        Label {
            width: firstColumnWidth
            text: qsTr("Region")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Language")
            horizontalAlignment: Label.AlignRight
        }
        ComboBox {
            width: secondColumnWidth
            model: ["简体中文", "English"]
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Appearance")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("System text rendering")
            horizontalAlignment: Label.AlignRight
        }
        Item {
            width: secondColumnWidth
            height: systemFontRenderingSwitch.height
            Switch {
                id: systemFontRenderingSwitch
                checked: false
            }
        }
    }
}