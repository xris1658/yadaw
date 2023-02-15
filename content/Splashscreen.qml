import QtQuick

Window {
    id: root
    property string text: qsTr("Starting...")
    flags: Qt.SplashScreen
    visible: true
    width: image.sourceSize.width
    height: image.sourceSize.height
    Image {
        id: image
        source: "./images/Splashscreen.png"
        Label {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: 24
            text: root.text
            verticalAlignment: Text.AlignLeft
            horizontalAlignment: Text.AlignBottom
        }
    }
}
