import QtQuick
import QtQuick.Layouts

Item {
    id: root
    height: 300

    property alias model: list.model

    ListView {
        id: list
        anchors.fill: parent
        spacing: 5
        delegate: Row {
            spacing: 3
            Item {
                id: checkBoxPlaceholder
                height: comboBoxButton.height
                width: height
                CheckBox {
                    id: checkBox
                    visible: !adioglm_is_main
                    checked: true
                    spacing: 0
                    anchors.centerIn: parent
                }
            }
            Label {
                text: (index + 1) + ". " + adioglm_name
                verticalAlignment: Qt.AlignVCenter
                width: list.width - checkBoxPlaceholder.width - comboBoxButton.width - parent.spacing * 2
                height: comboBoxButton.height
            }
            ComboBoxButton {
                id: comboBoxButton
                width: 100
            }
        }
    }
}