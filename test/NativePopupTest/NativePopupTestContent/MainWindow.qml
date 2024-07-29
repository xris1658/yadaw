import QtQuick
import QtQuick.Controls.Basic

Window {
    id: root
    width: 400
    height: 400
    visible: true
    color: mouseArea.containsMouse? "#252525": "#202020"
    property Window nativePopup: nativePopup
    signal showNativePopup(x: int, y: int)
    Item {
        // Since an inactive `QQuickWindow` does not receive key events even if
        // QQuickWindow::event() is called, we have to manually forward the
        // event to the native popup.
        id: keyboardForwarder
        property Item previousActiveFocusItem: null
        focus: false
        Keys.onPressed: (event) => {
            menu.keysPressed(event);
        }
        signal startForwarding(previousActiveFocusItem: Item)
        onStartForwarding: (previousActiveFocusItem) => {
            keyboardForwarder.previousActiveFocusItem = previousActiveFocusItem;
            keyboardForwarder.focus = true;
            keyboardForwarder.forceActiveFocus();
        }
        signal endForwarding()
        onEndForwarding: {
            keyboardForwarder.focus = false;
            keyboardForwarder.previousActiveFocusItem.forceActiveFocus();
        }
    }
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
            keyboardForwarder.startForwarding(mouseArea);
            showNativePopup(globalPoint.x, globalPoint.y);
            nativePopup.visible = true;
        }
        Rectangle {
            width: 10
            height: 10
            color: "#FF0000"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 5
            visible: mouseArea.activeFocus
        }
    }
    Button {
        id: button
        width: 40
        height: 40
        anchors.centerIn: parent
        z: 2
        focus: true
        onClicked: {
            console.log("clicked");
        }
        Keys.onMenuPressed: {
            let globalPoint = mapToGlobal(width / 2, height / 2);
            if(globalPoint.y + nativePopup.height >= root.screen.desktopAvailableHeight) {
                globalPoint.y -= nativePopup.height;
                if(globalPoint.y < 0) {
                    globalPoint.y = root.screen.desktopAvailableHeight - nativePopup.height;
                }
            }
            globalPoint.x = Math.min(globalPoint.x, root.screen.desktopAvailableWidth - nativePopup.width);
            nativePopup.x = globalPoint.x;
            nativePopup.y = globalPoint.y;
            keyboardForwarder.startForwarding(button);
            showNativePopup(globalPoint.x, globalPoint.y);
            nativePopup.visible = true;
        }
        Rectangle {
            width: 10
            height: 10
            color: "#FF0000"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 5
            visible: button.activeFocus
        }
    }
    Window {
        id: nativePopup
        flags: Qt.Tool | Qt.FramelessWindowHint
        visible: false
        width: 200
        height: Math.min(menu.implicitHeight, nativePopup.screen.desktopAvailableHeight)
        color: "#202020"
        Menu {
            id: menu
            width: 200
            height: nativePopup.height
            visible: nativePopup.visible
            padding: 1
            Repeater {
                model: 25
                MenuItem {
                    width: menu.width - menu.padding * 2
                    height: 20
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
                    background: Rectangle {
                        color: parent.pressed? "#002040":
                            menu.currentIndex == index? "#0050A0": "transparent"
                    }
                    onClicked: {
                        button.text = (index + 1);
                        nativePopup.hide();
                    }
                }
            }
            background: Rectangle {
                color: "transparent"
                border.color: "#808080"
                border.width: menu.padding
            }
            signal keysPressed(event: var)
            onKeysPressed: (event) => {
                let accepted = false;
                if(event.key == Qt.Key_Up
                    || event.key == Qt.Key_Backtab
                    || ((event.key == Qt.Key_Tab && event.modifiers == Qt.ShiftModifier))) {
                    if(menu.currentIndex <= 0) {
                        menu.currentIndex = menu.count - 1;
                    }
                    else {
                        --menu.currentIndex;
                    }
                    accepted = true;
                }
                else if(event.key == Qt.Key_Down
                    || ((event.key == Qt.Key_Tab && event.modifiers == Qt.NoModifier))) {
                    if(menu.currentIndex == -1 || menu.currentIndex == menu.count - 1) {
                        menu.currentIndex = 0;
                    }
                    else {
                        ++menu.currentIndex;
                    }
                    accepted = true;
                }
                else if(event.key == Qt.Key_Return) {
                    if(menu.currentIndex != -1) {
                        menu.itemAt(menu.currentIndex).clicked();
                        accepted = true;
                    }
                }
                else if(event.key == Qt.Key_Escape) {
                    nativePopup.hide();
                }
                event.accepted = accepted;
            }
        }
        signal mousePressedOutside()
        onMousePressedOutside: {
            hide();
        }
        onVisibleChanged: {
            if(!visible) {
                keyboardForwarder.endForwarding();
            }
        }
    }
    onActiveChanged: {
        // Workaround on X11 since I don't know how to prevent the fake menu
        // from take focus from the main window
        if(!active) {
            nativePopup.hide();
        }
    }
    Component.onCompleted: {
        mouseArea.forceActiveFocus();
    }
}