import QtQml
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("About YADAW")
    modality: Qt.WindowModal
    color: Colors.background

    width: content.width + content.spacing * 2
    height: content.height + content.spacing * 3 + buttonOk.height

    Row {
        id: content
        anchors.top: parent.top
        anchors.topMargin: spacing
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        Image {
            width: 64
            height: width
            source: "./images/Main-icon.ico"
            smooth: true
            mipmap: true
        }
        Column {
            spacing: 5
            Label {
                text: "YADAW"
                font.bold: true
                color: Colors.content
                font.pointSize: Qt.application.font.pointSize * 1.6
            }
            Label {
                text: "Yet Another Digital Audio Workstation"
                color: Colors.content
            }
            Label {
                text: "Development Version"
                color: Colors.secondaryContent
            }
            Label {
                text: "Copyright (c) 2021-2023 xris1658. All rights reserved (for now)."
                color: Colors.content
            }
        }
    }
    Button {
        id: buttonOk
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: content.spacing
        text: Constants.okTextWithMnemonic
        onClicked: {
            root.close();
        }
    }
    Button {
        id: buttonThirdPartySoftware
        anchors.right: buttonOk.left
        anchors.rightMargin: content.spacing
        anchors.verticalCenter: buttonOk.verticalCenter
        text: qsTr("&Powered by...")
        onClicked: {
            let component = Qt.createComponent("ThirdPartySoftware.qml");
            if(component.status === Component.Ready) {
                let window = component.createObject(root);
                window.showNormal();
            }
        }
    }
    Label {
        text: "<a href=\"https://github.com/xris1658/yadaw\">GitHub</a>"
        anchors.right: buttonThirdPartySoftware.left
        anchors.rightMargin: content.spacing
        anchors.verticalCenter: buttonOk.verticalCenter
        onLinkActivated: (link) => {
            Qt.openUrlExternally(link);
        }
    }

    Component.onCompleted: {
        buttonOk.forceActiveFocus();
    }
}
