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
    width: image.sourceSize.width / root.screen.devicePixelRatio
    height: image.sourceSize.height / root.screen.devicePixelRatio
    Image {
        id: image
        source: "./images/Splashscreen.png"
        anchors.fill: parent
        fillMode: Image.Stretch
        Label {
            id: label
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 25 / root.screen.devicePixelRatio
            text: qsTr("Starting...")
            font.pixelSize: 14 / root.screen.devicePixelRatio
            verticalAlignment: Text.AlignLeft
            horizontalAlignment: Text.AlignBottom
        }
    }
}
