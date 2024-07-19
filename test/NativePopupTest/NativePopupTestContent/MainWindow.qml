import QtQuick
import QtQuick.Controls.Basic

Window {
    id: root
    width: 400
    height: 400
    visible: true
    color: mouseArea.containsMouse? "#333333": "#202020"
    property Window nativePopup: nativePopup
    signal showNativePopup(x: int, y: int)
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.RightButton
        onReleased: (mouse) => {
            let globalPoint = mapToGlobal(mouse.x, mouse.y);
            nativePopup.x = globalPoint.x;
            nativePopup.y = globalPoint.y;
            showNativePopup(globalPoint.x, globalPoint.y);
        }
    }
    Window {
        id: nativePopup
        flags: Qt.FramelessWindowHint
        visible: false
        width: 200
        height: 325
        color: "#202020"
        Rectangle {
            z: 2
            anchors.fill: parent
            color: "transparent"
            border.color: "#555555"
        }
        Button {
            z: 1
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: 5
            anchors.topMargin: 5
            width: 15
            height: 15
            onClicked: {
                nativePopup.hide();
            }
            Text {
                anchors.centerIn: parent
                text: "x"
            }
        }
        Column {
            x: 0
            y: 25
            z: 1
            Repeater {
                model: 15
                MouseArea {
                    width: nativePopup.width
                    height: 20
                    acceptedButtons: Qt.LeftButton
                    hoverEnabled: true
                    Text {
                        x: 3
                        z: 2
                        width: parent.width - x
                        height: parent.height
                        text: "Button " + (index + 1)
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        color: "#FFFFFF"
                    }
                    Rectangle {
                        anchors.fill: parent
                        color: parent.pressed? "#002040":
                            parent.containsMouse? "#0050A0": "transparent"
                    }
                    onClicked: {
                        nativePopup.hide();
                    }
                }
            }
        }
        onActiveChanged: {
            if(!active) {
                hide();
            }
        }
    }
}