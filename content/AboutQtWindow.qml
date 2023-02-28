import QtQml
import QtQuick
import QtQuick.Window

Window {
    id: root
    flags: Qt.Dialog
    title: qsTr("About Qt")
    modality: Qt.WindowModal
    color: Colors.background

    width: 600
    height: content.height + content.spacing * 3 + buttonOk.height

    Row {
        id: content
        anchors.top: parent.top
        anchors.topMargin: spacing
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        Image {
            id: icon
            width: 64
            height: width
            source: "./images/qtlogo-64.png"
            smooth: true
            mipmap: true
        }
        Column {
            spacing: 5
            Label {
                text: "About Qt"
                font.bold: true
                color: Colors.content
                font.pointSize: Qt.application.font.pointSize * 1.6
            }
            Label {
                text: "This program uses Qt version %1.".arg(Global.qtVersion)
                color: Colors.content
            }
            // Source: <Qt install path>\6.4.2\Src\src\widgets\dialogs\qmessagebox.cpp:1843
            // There seems to be no way to create a "About Qt" dialog without copying
            // the following text or introducing Qt Widgets.
            Label {
                text:
                    "<p>Qt is a C++ toolkit for cross-platform application
                    development.</p>
                    <p>Qt provides single-source portability across all major desktop
                    operating systems. It is also available for embedded Linux and other
                    embedded and mobile operating systems.</p>
                    <p>Qt is available under multiple licensing options designed
                    to accommodate the needs of our various users.</p>
                    <p>Qt licensed under our commercial license agreement is appropriate
                    for development of proprietary/commercial software where you do not
                    want to share any source code with third parties or otherwise cannot
                    comply with the terms of GNU (L)GPL.</p>
                    <p>Qt licensed under GNU (L)GPL is appropriate for the
                    development of Qt&nbsp;applications provided you can comply with the terms
                    and conditions of the respective licenses.</p>
                    <p>Please see <a href=\"http://%2/\">%2</a>
                    for an overview of Qt licensing.</p>
                    <p>Copyright (C) %1 The Qt Company Ltd and other
                    contributors.</p>
                    <p>Qt and the Qt logo are trademarks of The Qt Company Ltd.</p>
                    <p>Qt is The Qt Company Ltd product developed as an open source
                    project. See <a href=\"http://%3/\">%3</a> for more information.</p>"
                   .arg("2022").arg("qt.io/licensing").arg("qt.io")
                color: Colors.content
                width: root.width - content.spacing * 3 - icon.width
                wrapMode: Text.WordWrap
                onLinkActivated: (link) => {
                    Qt.openUrlExternally(link);
                }
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
    Component.onCompleted: {
        buttonOk.forceActiveFocus();
    }
    onClosing: {
        EventSender.darkModeSupportWindow = root;
        EventSender.removeWindowForDarkModeSupport();
    }
}
