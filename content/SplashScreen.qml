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
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 33
            text: qsTr("Starting...")
            font.pixelSize: 14
            verticalAlignment: Text.AlignLeft
            horizontalAlignment: Text.AlignBottom
        }
    }
}
