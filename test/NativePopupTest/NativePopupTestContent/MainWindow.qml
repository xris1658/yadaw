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
        z: 1
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.RightButton
        onReleased: (mouse) => {
            let globalPoint = mapToGlobal(mouse.x, mouse.y);
            if(globalPoint.y + nativePopup.height >= root.screen.desktopAvailableHeight) {
                globalPoint.y -= nativePopup.height;
                if(globalPoint.y < 0) {
                    globalPoint.y = root.screen.desktopAvailableHeight - nativePopup.height;
                }
            }
            globalPoint.x = Math.min(globalPoint.x, root.screen.desktopAvailableWidth - nativePopup.width);
            nativePopup.x = globalPoint.x;
            nativePopup.y = globalPoint.y;
            showNativePopup(globalPoint.x, globalPoint.y);
            nativePopup.visible = true;
        }
    }
    Button {
        id: button
        width: 40
        height: 40
        anchors.centerIn: parent
        z: 2
        onClicked: {
            console.log("clicked");
        }
    }
    Window {
        id: nativePopup
        flags: Qt.Tool | Qt.FramelessWindowHint
        visible: false
        width: 200
        height: column.height
        color: "#202020"
        Rectangle {
            z: 2
            anchors.fill: parent
            color: "transparent"
            border.color: "#555555"
        }
        Column {
            id: column
            x: 0
            y: 0
            z: 1
            Repeater {
                model: 25
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
                        button.text = (index + 1);
                        nativePopup.hide();
                    }
                }
            }
        }
        signal mousePressedOutside()
        onMousePressedOutside: {
            hide();
        }
    }
    onActiveChanged: {
        // Workaround on X11 since I don't know how to prevent the fake menu
        // from take focus from the main window
        if(!active) {
            nativePopup.hide();
        }
    }
}