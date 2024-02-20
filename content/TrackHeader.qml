import QtQuick

Rectangle {
    id: root
    property int trackHeaderIndex
    property alias name: nameText.text
    property alias trackColor: header.color
    color: Colors.background
    readonly property Item header: header

    signal setName(name: string)
    signal startRename()

    Rectangle {
        id: header
        width: indexText.width
        height: root.height
        anchors.left: parent.left
        Label {
            id: indexText
            font.pointSize: Qt.application.font.pointSize * 0.75
            width: contentHeight * 3
            verticalAlignment: Label.AlignVCenter
            horizontalAlignment: Label.AlignHCenter
            topPadding: 3
            bottomPadding: 3
            text: root.trackHeaderIndex + 1
        }
    }
    Item {
        id: content
        width: root.width - header.width - 3
        height: root.height
        anchors.right: parent.right
        Row {
            Column {
                Label {
                    id: nameText
                    width: content.width
                    verticalAlignment: Label.AlignVCenter
                    horizontalAlignment: Label.AlignLeft
                    topPadding: 3
                    bottomPadding: 3
                    TextField {
                        id: nameTextField
                        anchors.fill: parent
                        anchors.leftMargin: leftPadding * -1
                        z: 2
                        text: nameText.text
                        visible: false
                        padding: 0
                        Keys.onEscapePressed: (event) => {
                            visible = false;
                        }
                        onAccepted: {
                            root.setName(nameTextField.text);
                            visible = false;
                        }
                    }
                    MouseArea {
                        id: nameMouseArea
                        anchors.fill: parent
                        z: 1
                        acceptedButtons: Qt.LeftButton
                        onDoubleClicked: {
                            root.startRename();
                        }
                    }
                }
                Row {
                    spacing: 3
                    Button {
                        width: height
                        text: "M"
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        width: height
                        text: "S"
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        width: height
                        text: "\u2205"
                        topPadding: 1
                        bottomPadding: 1
                    }
                    Button {
                        width: height
                        text: "R"
                        topPadding: 1
                        bottomPadding: 1
                    }
                }
            }
        }
    }
    onStartRename: {
        nameTextField.selectAll();
        nameTextField.forceActiveFocus();
        nameTextField.visible = true;
    }
}
