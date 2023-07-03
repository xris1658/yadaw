import QtQuick
import QtQuick.Shapes

ApplicationWindow {
    id: root
    width: 840
    height: 720
    visible: true

    menuBar: MenuBar {
        Menu {
            title: "&Qt6Playground"
            Action {
                text: "&About Qt6Playground..."
            }
            MenuSeparator {}
            Action {
                text: "E&xit"
                onTriggered: {
                    Qt.quit();
                }
            }
        }
        Menu {
            title: "&File"
            Action {
                text: "&New"
            }
            Action {
                text: "&Open"
            }
            Menu {
                title: "&Recent Files"
                Action {
                    text: "Empty list"
                    enabled: false
                }
                MenuSeparator {}
                Action {
                    text: "Clear list"
                }
            }
            MenuSeparator {}
            Action {
                text: "&Save"
            }
            Action {
                text: "Save &As..."
            }
            Action {
                text: "Auto Sa&ve"
                checkable: true
                checked: false
            }
            Action {
                text: "&Revert"
            }
            MenuSeparator {}
            Menu {
                title: "&Export"
                Action {
                    text: "&Mixdown..."
                }
                Action {
                    text: "&Separated tracks..."
                }
            }
            MenuSeparator {}
            Action {
                text: "&Project Settings..."
            }
        }
        Menu {
            title: "Test"
            Repeater {
                model: 100
                MenuItem {
                    text: (modelData + 1).toString()
                }
            }
        }
    }

    Rectangle {
        id: mnemonicIndicator
        width: 10
        height: 10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 5
        color: "#FF0000"
    }

    Column {
        anchors.centerIn: parent
        spacing: 10
        Row {
            spacing: 5
            Button {
                text: "Default"
            }
            Button {
                mnemonicTextLook: Mnemonic.MnemonicDisabled
                text: "&Mnemonic Test"
                onClicked: {
                    mnemonicIndicator.visible = !mnemonicIndicator.visible;
                }
            }
            Button {
                mnemonicTextLook: Mnemonic.MnemonicEnabled
                text: "M&nemonic Test"
                onClicked: {
                    mnemonicIndicator.visible = !mnemonicIndicator.visible;
                }
            }
            Button {
                mnemonicTextLook: Mnemonic.MnemonicEnabledWithUnderline
                text: "Mn&emonic Test"
                onClicked: {
                    mnemonicIndicator.visible = !mnemonicIndicator.visible;
                }
            }
            Button {
                width: 200
                text: "Explicit width"
            }
            Button {
                width: 50
                text: "Explicit width (2)"
            }
            Button {
                height: 30
                text: "Explicit height"
            }
            Button {
                enabled: false
                text: "Disabled"
            }
        }
        Column {
            spacing: 5
            CheckBox {
                checked: false
                text: "Unchecked"
            }
            CheckBox {
                checked: true
                text: "Checked"
            }
            CheckBox {
                tristate: true
                checkState: Qt.PartiallyChecked
                text: "Partially Checked"
            }
            CheckBox {
                checked: true
                enabled: false
                text: "Checked (disabled)"
            }
            CheckBox {
                tristate: true
                checkState: Qt.PartiallyChecked
                enabled: false
                text: "Partially Checked (disabled)"
            }
            CheckBox {
                checked: true
                enabled: false
                text: "Checked (disabled)"
            }
            CheckBox {
                width: 50
                text: "Checked (explicit width)"
            }
            CheckBox {
                height: 50
                text: "Checked (explicit height)"
            }
        }
        Row {
            spacing: 16
            Rectangle {
                width: colorPicker.width
                height: colorPicker.height
                color: "transparent"
                border.color: Colors.controlBorder
                Flickable {
                    clip: true
                    width: parent.width
                    height: parent.height
                    contentWidth: item.width
                    contentHeight: item.height
                    interactive: true
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }
                    Rectangle {
                        id: item
                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#6600ff"
                            }
                            GradientStop {
                                position: 1
                                color: "#ff0066"
                            }
                        }
                        width: 320
                        height: 320
                    }
                }
            }
            Row {
                ColorPicker {
                    id: colorPicker
                    color: "#C00000"
                }
                Slider {
                    orientation: Qt.Vertical
                    length: colorPicker.radius * 2
                }
            }
        }
        Row {
            spacing: 5
            ComboBox {
                model: ["a", "b", "c"]
            }
            ComboBox {
                width: 50
                model: ["a", "b", "c"]
            }
            ComboBox {
                enabled: false
                model: ["a", "b", "c"]
            }
            ComboBox {
                editable: true
                model: ["123456", "123456", "123456"]
            }
        }
        Row {
            spacing: 5
            ProgressBar {
                value: 0.5
            }
            ProgressBar {
                progressState: ProgressBar.Paused
                value: 0.5
            }
            ProgressBar {
                progressState: ProgressBar.Stopped
                value: 0.5
            }
            ProgressBar {
                indeterminate: true
            }
        }
        ProgressBar {
            width: 500
            indeterminate: true
        }
        Row {
            TextField {
                id: textField
                width: 200
                placeholderText: "abcd"
            }
        }
        Row {
            Dial {
                diameter: 40
            }
        }
    }
}
