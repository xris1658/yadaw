import QtQml
import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property alias font: digit.font
    property color color: Colors.bigClockText

    width: properties.digitWidth * 12
    height: digit.contentHeight

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
        property int digitWidth: digit.contentWidth
        property string textOfBeatTime: bar.toString().padStart(4, '0') + ":" + beat.toString().padStart(2, '0') + "." + pulse.toString().padStart(4, '0')
        property string textOfActualTime: hour.toString().padStart(2, '0') + ":" + minute.toString().padStart(2, '0') + ":" + second.toString().padStart(2, '0') + "." + millisecond.toString().padStart(3, '0')
    }

    Label {
        id: digit
        clip: true
        width: 0
        height: 0
        text: "0"
    }

    StackLayout {
        z: 2
        width: digit.contentWidth * 12
        height: digit.contentHeight
        currentIndex: root.timeFormat
        anchors.centerIn: parent
        Item {
            id: beatTimeItem
            width: properties.bigClockDigitWidth * 12
            height: digit.contentHeight
            Layout.alignment: Layout.AlignVCenter | Layout.AlignHCenter
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    model: 12
                    Label {
                        color: root.color
                        width: (index === 4 || index === 7)? contentWidth: properties.digitWidth
                        height: root.height
                        horizontalAlignment: Text.AlignHCenter
                        text: properties.textOfBeatTime.charAt(index)
                        font: digit.font
                    }
                }
            }
        }
        Item {
            id: actualTimeItem
            width: properties.bigClockDigitWidth * 12
            height: digit.contentHeight
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    model: 12
                    Label {
                        color: root.color
                        width: (index === 2 || index === 5 || index === 8)? contentWidth: properties.digitWidth
                        height: root.height
                        horizontalAlignment: Text.AlignHCenter
                        text: properties.textOfActualTime.charAt(index)
                        font: digit.font
                    }
                }
            }
        }
        Item {
            width: properties.bigClockDigitWidth * sampleAsString.length
            height: digit.contentHeight
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                Repeater {
                    model: sampleAsString.length
                    Label {
                        color: root.color
                        width: properties.digitWidth
                        height: root.height
                        horizontalAlignment: Text.AlignHCenter
                        text: sampleAsString.charAt(index)
                        font: digit.font
                    }
                }
            }
        }
    }
}
