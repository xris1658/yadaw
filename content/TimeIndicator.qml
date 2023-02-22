import QtQml
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    width: properties.bigDigitWidth * 12
    height: bigDigit.contentHeight

    enum TimeFormat {
        Beat,
        Second,
        Sample
    }

    property int timeFormat: TimeIndicator.Beat

    property int bar: 1
    property int beat: 1
    property int pulse: 0

    property int hour: 0
    property int minute: 0
    property int second: 0
    property int millisecond: 0

    property string sampleAsString: "0"

    QtObject {
        id: properties
        property int bigDigitWidth: bigDigit.contentWidth
        property int smallDigitWidth: smallDigit.contentWidth
        property string textOfBeatTime: bar.toString().padStart(4, '0') + ":" + beat.toString().padStart(2, '0') + "." + pulse.toString().padStart(4, '0')
        property string textOfActualTime: hour.toString().padStart(2, '0') + ":" + minute.toString().padStart(2, '0') + ":" + second.toString().padStart(2, '0') + "." + millisecond.toString().padStart(3, '0')
    }

    Label {
        id: smallDigit
        clip: true
        width: 0
        height: 0
        text: "0"
    }

    Label {
        id: bigDigit
        clip: true
        width: 0
        height: 0
        text: "0"
        font.bold: false
        font.family: "Fira Sans Condensed"
        font.pointSize: Qt.application.font.pointSize * 2.5
    }

    StackLayout {
        width: bigDigit.contentWidth * 12
        height: bigDigit.contentHeight
        currentIndex: root.timeFormat
        anchors.centerIn: parent
        Item {
            id: beatTimeItem
            width: properties.bigClockDigitWidth * 12
            height: bigDigit.contentHeight
            Layout.alignment: Layout.AlignVCenter | Layout.AlignHCenter
            ListView {
                anchors.horizontalCenter: parent.horizontalCenter
                width: contentWidth
                height: contentHeight
                model: 12
                orientation: Qt.Horizontal
                delegate: Label {
                    color: Colors.bigClockTextColor
                    width: (index === 4 || index === 7)? contentWidth: properties.bigDigitWidth
                    horizontalAlignment: Text.AlignHCenter
                    text: properties.textOfBeatTime.charAt(index)
                    font.bold: false
                    font.family: "Fira Sans Condensed"
                    font.pointSize: Qt.application.font.pointSize * 2.5
                }
            }
        }
        Item {
            id: actualTimeItem
            width: properties.bigClockDigitWidth * 12
            height: bigDigit.contentHeight
            ListView {
                anchors.horizontalCenter: parent.horizontalCenter
                width: contentWidth
                height: contentHeight
                model: 12
                orientation: Qt.Horizontal
                delegate: Label {
                    color: Colors.bigClockTextColor
                    width: (index === 2 || index === 5 || index === 8)? contentWidth: properties.bigDigitWidth
                    height: bigDigit.height
                    horizontalAlignment: Text.AlignHCenter
                    text: properties.textOfActualTime.charAt(index)
                    font.bold: false
                    font.family: "Fira Sans Condensed"
                    font.pointSize: Qt.application.font.pointSize * 2.5
                }
            }
        }
        Item {
            width: properties.bigClockDigitWidth * sampleAsString.length
            height: bigDigit.contentHeight
            ListView {
                anchors.horizontalCenter: parent.horizontalCenter
                width: contentWidth
                height: contentHeight
                model: sampleAsString.length
                orientation: Qt.Horizontal
                delegate: Label {
                    color: Colors.bigClockTextColor
                    width: properties.bigDigitWidth
                    height: bigDigit.height
                    horizontalAlignment: Text.AlignHCenter
                    text: sampleAsString.charAt(index)
                    font.bold: false
                    font.family: "Fira Sans Condensed"
                    font.pointSize: Qt.application.font.pointSize * 2.5
                }
            }
        }
    }
}
