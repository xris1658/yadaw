import QtQml
import QtQuick

Item {
    id: root
    property int cpuUsagePercentage: 10
    width: Math.max(timeSig.x, timeSigLabel.x) - cpuMeter.x + cpuMeter.width
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
            id: cpuMeter
            width: 50 + border.width * 2
            height: cpuPercentage.contentHeight + 6
            color: Colors.background
            border.color: Colors.controlBorder
            Rectangle {
                width: Math.max(0, Math.min(1, cpuUsagePercentage * 0.01) * (parent.width - parent.border.width * 2))
                height: parent.height - parent.border.width * 2
                x: parent.border.width
                y: parent.border.width
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
            color: Colors.secondaryContent
        }
        Column {
            TimeIndicator {
                color: Colors.content
            }
            TimeIndicator {
                color: Colors.content
            }
        }
        Label {
            text: "LOOP"
            color: Colors.secondaryContent
        }
        TimeIndicator {
            font.bold: false
            font.family: "Fira Sans Condensed"
            font.pointSize: Qt.application.font.pointSize * 2.5
            color: Colors.bigClockText
            id: timeIndicator
        }
        Label {
            id: timeIndicatorText
            text: "TIME"
            color: Colors.secondaryContent
        }
        Label {
            text: "128.000"
            font.pointSize: Qt.application.font.pointSize * 2
        }
        Label {
            text: "BPM"
            color: Colors.secondaryContent
        }
        Row {
            id: timeSig
            Label {
                text: "6"
                height: timeSigSlash.height
                verticalAlignment: Label.AlignTop
                font.pointSize: Qt.application.font.pointSize * 1.5
            }
            Label {
                id: timeSigSlash
                text: "/"
                font.pointSize: Qt.application.font.pointSize * 2
            }
            Label {
                text: "8"
                height: timeSigSlash.height
                verticalAlignment: Label.AlignBottom
                font.pointSize: Qt.application.font.pointSize * 1.5
            }
        }
        Label {
            id: timeSigLabel
            text: "TIME SIG."
            color: Colors.secondaryContent
        }
    }
}
