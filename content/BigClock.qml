import QtQml
import QtQuick

Item {
    property int cpuUsagePercentage: 10
    height: timeIndicator.height +timeIndicatorText.height
    Grid {
        anchors.centerIn: parent
        horizontalItemAlignment: Grid.AlignHCenter
        verticalItemAlignment: Grid.AlignVCenter
        flow: Grid.TopToBottom
        rowSpacing: 0
        columnSpacing: 5
        rows: 2
        Rectangle {
            width: 100
            height: cpuPercentage.contentHeight + 6
            color: Colors.background
            Rectangle {
                width: Math.max(0, Math.min(100, cpuUsagePercentage))
                height: parent.height
                z: 1
                color: Colors.controlBorder
            }
            Label {
                id: cpuPercentage
                z: 2
                text: cpuUsagePercentage + "%"
                anchors.centerIn: parent
            }
        }
        Label {
            text: "CPU"
        }
        Label {
            text: "   1: 1: 0.   0\n   5: 1: 0.   0"
        }
        Label {
            text: "LOOP"
        }
        TimeIndicator {
            id: timeIndicator
        }
        Label {
            id: timeIndicatorText
            text: "TIME"
        }
        Label {
            text: "128.000"
            font.pointSize: Qt.application.font.pointSize * 2
        }
        Label {
            text: "BPM"
        }
        Label {
            text: "16 / 16"
            font.pointSize: Qt.application.font.pointSize * 2
        }
        Label {
            text: "TIME SIG."
        }
    }
}