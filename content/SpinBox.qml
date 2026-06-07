import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T

T.SpinBox {
    id: root

    topPadding: 3 // TODO: Use paddings
    bottomPadding: 3
    leftPadding: 5
    rightPadding: 5
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: contentItem.implicitWidth
    implicitHeight: contentItem.implicitHeight

    up.hovered: upButton.hovered
    up.indicator: upButton
    up.pressed: upButton.pressed

    down.hovered: downButton.hovered
    down.indicator: downButton
    down.pressed: downButton.pressed

    background: Item {
        id: background
        width: root.width
        height: root.height
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
    }
    contentItem: TextField {
        id: contentItem
        anchors.fill: parent
        anchors.rightMargin: root.rightInset + upDownPlaceholder.width - 1
        padding: 0
        z: 2
        leftInset: root.leftInset
        rightInset: 0
        topInset: root.topInset
        bottomInset: root.bottomInset
        text: root.textFromValue(root.value, root.locale)
        validator: root.validator
        readOnly: !root.editable
        inputMethodHints: root.inputMethodHints
        Keys.onUpPressed: {
            root.increase();
        }
        Keys.onDownPressed: {
            root.decrease();
        }
    }
    Rectangle {
        id: upDownPlaceholder
        width: height
        anchors.right: parent.right
        anchors.rightMargin: anchors.rightInset
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: root.enabled? Colors.controlBackground: Colors.background
        border.color: root.enabled? Colors.controlBorder: Colors.disabledControlBorder
        z: 1
        Button {
            id: upButton
            flat: true
            anchors.fill: parent
            anchors.margins: 1
            anchors.bottomMargin: parent.height / 2
            text: "\u25b4"
            enabled: root.enabled && root.value < root.to
            focusPolicy: Qt.NoFocus
            onClicked: {
                root.increase();
                root.forceActiveFocus();
            }
        }
        Button {
            id: downButton
            flat: true
            anchors.fill: parent
            anchors.margins: 1
            anchors.topMargin: parent.height / 2
            text: "\u25be"
            enabled: root.enabled && root.value > root.from
            focusPolicy: Qt.NoFocus
            onClicked: {
                root.decrease();
                root.forceActiveFocus();
            }
        }
    }
    onActiveFocusChanged: {
        if(activeFocus) {
            contentItem.forceActiveFocus;
        }
    }
}
