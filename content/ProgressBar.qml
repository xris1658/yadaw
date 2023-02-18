import QtQuick
import QtQuick.Templates as T

T.ProgressBar {
    id: root

    enum ProgressState {
        Running,
        Paused,
        Stopped
    }
    property int progressState: ProgressBar.Running

    padding: 1
    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    implicitWidth: 100
    implicitHeight: 20

    background: Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        color: root.enabled? Colors.controlBackground: Colors.background
        border.color: root.enabled? Colors.controlBorder: Colors.disabledControlBorder
    }
    contentItem: Item {
        id: content
        anchors.fill: parent
        anchors.topMargin: root.topPadding
        anchors.bottomMargin: root.bottomPadding
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.rightPadding
        clip: true
        Rectangle {
            visible: !root.indeterminate
            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width * root.position
            height: parent.height
            color: root.progressState === ProgressBar.Running? Colors.progressBarRunningColor:
                root.progressState === ProgressBar.Paused? Colors.progressBarPausedColor:
                root.progressState === ProgressBar.Stopped? Colors.progressBarStoppedColor: "transparent"
        }
        Rectangle {
            id: indeterminateIndicator
            visible: root.indeterminate
            width: parent.width * 0.5
            height: parent.height
            gradient: Gradient {
                orientation: Qt.Horizontal
                GradientStop {
                    position: 0
                    color: "transparent"
                }
                GradientStop {
                    position: 0.5
                    color: Colors.progressBarRunningColor
                }
                GradientStop {
                    position: 1
                    color: "transparent"
                }
            }
            NumberAnimation on x {
                running: root.enabled && root.visible && root.indeterminate
                from: -1 * indeterminateIndicator.width
                to: content.width
                loops: Animation.Infinite
                duration: 2000
            }
        }
    }
}