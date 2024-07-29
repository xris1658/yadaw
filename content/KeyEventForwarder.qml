import QtQuick

Item {
    id: root
    property Item previousActiveFocusItem: null
    focus: false
    signal startForwarding(previousActiveFocusItem: Item)
    onStartForwarding: (previousActiveFocusItem) => {
        root.previousActiveFocusItem = previousActiveFocusItem;
        root.focus = true;
        root.forceActiveFocus();
    }
    signal endForwarding()
    onEndForwarding: {
        root.focus = false;
        if(root.previousActiveFocusItem) {
            root.previousActiveFocusItem.forceActiveFocus();
        }
    }
    signal keysPressed(event: var)
    Keys.onPressed: (event) => {
        keysPressed(event);
    }
}