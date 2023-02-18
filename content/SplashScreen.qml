import QtQuick

Window {
    id: root

    signal setText(text: string)
    onSetText: (text) => {
        label.text = text;
    }
    signal closeSplashScreen()
    onCloseSplashScreen: {
        root.close();
    }
    flags: Qt.SplashScreen
    visible: true
    width: image.sourceSize.width
    height: image.sourceSize.height
    Image {
        id: image
        source: "./images/Splashscreen.png"
        Label {
            id: label
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: 24
            text: qsTr("Starting...")
            verticalAlignment: Text.AlignLeft
            horizontalAlignment: Text.AlignBottom
        }
    }
}
