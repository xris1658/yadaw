import QtQuick
import QtQuick.Shapes

ApplicationWindow {
    id: root
    width: 840
    height: 640
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
                width: 50
                text: "Checked (disabled)"
            }
        }
        Rectangle {
            width: 200
            height: 200
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
                    width: 200
                    height: 300
                }
            }
        }
        ComboBox {
            model: ["a", "b", "c"]
        }
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
        ProgressBar {
            width: 500
            indeterminate: true
        }
        Item {
            width: 60
            height: 72

            layer.enabled: true
            layer.smooth: true
            layer.samples: 4
            LoopIcon {
                id: loopIcon
                anchors.centerIn: parent
                scale: 0.25
                path.strokeColor: "transparent"
                path.fillColor: Colors.secondaryContent
                path.joinStyle: ShapePath.MiterJoin
            }
        }
        Item {
            width: 60
            height: 35
            PlayIcon {
                id: playIcon
                anchors.centerIn: parent
                scale: 0.25
                path.strokeColor: "transparent"
                path.fillColor: "#00FD00"
                path.joinStyle: ShapePath.MiterJoin

            }
        }
    }
}
