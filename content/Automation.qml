import QtQuick
import QtQuick.Controls
import QtQuick.Shapes

Item {
    id: root
    property double zoom: 1
    property int pointDiameter: 9
    property alias model: points.model
    Repeater {
        id: points
        Item {
            width: 1
            height: width
            x: zoom * am_time
            y: root.height * (1 - am_value)
            Rectangle {
                width: root.pointDiameter
                height: width
                anchors.centerIn: parent
                color: "transparent"
                border.width: 2
                border.color: Colors.automationColor
                radius: width / 2
            }
        }
    }
    Repeater {
        id: curves
        model: points.model
        Shape {
            ShapePath {
                startX: 0
                startY: 0
                strokeColor: Colors.automationColor
                strokeWidth: 2
                PathQuad {
                    x: zoom * am_time
                    y: root.height * (1 - am_value)
                }
            }
        }
    }
    Shape {
        ShapePath {
            startX: 0
            startY: 0
            strokeColor: Colors.automationColor
            strokeWidth: 2
            PathQuad {
                x: root.width
                y: parent.startY
            }
        }
    }
}
