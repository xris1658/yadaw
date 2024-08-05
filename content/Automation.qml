import QtQuick
import QtQuick.Controls
import QtQuick.Shapes

Item {
    id: root
    property double zoom: 1
    property int pointDiameter: 9
    property var model
    QtObject {
        id: impl
        function normalizeValue(automationModel, value: real): real {
            return (value - automationModel.minValue) / (automationModel.maxValue - automationModel.minValue)
        }
    }

    Repeater {
        z: 3
        id: points
        model: root.model
        Item {
            width: 1
            height: width
            x: zoom * am_time
            y: root.height * (1 - impl.normalizeValue(root.model, am_value))
            Rectangle {
                width: root.pointDiameter
                height: width
                anchors.centerIn: parent
                color: Colors.automationColor
                radius: width / 2
            }
        }
        Component.onCompleted: {
            curves.model = points.model;
        }
    }
    Repeater {
        z: 2
        id: curves
        Shape {
            id: shape
            property int curveIndex: index
            property alias startX: path.startX
            property alias startY: path.startY
            property alias endX: pathQuad.x
            property alias endY: pathQuad.y
            ShapePath {
                id: path
                startX: shape.curveIndex > 0? points.itemAt(shape.curveIndex - 1).x: 0
                startY: shape.curveIndex > 0? points.itemAt(shape.curveIndex - 1).y: pathQuad.y
                strokeColor: Colors.automationColor
                strokeWidth: 2
                PathQuad {
                    id: pathQuad
                    x: zoom * am_time
                    y: root.height * (1 - impl.normalizeValue(root.model, am_value))
                    controlX: (path.startX + x) / 2
                    controlY: shape.curveIndex == 0? y:
                        0.5 * ((path.startY + y) - am_curve * Math.abs(shape.endY - shape.startY))
                }
            }
        }
    }
    Shape {
        z: 1
        ShapePath {
            id: endPath
            startX: points.itemAt(points.count - 1).x
            startY: points.itemAt(points.count - 1).y
            strokeColor: Colors.automationColor
            strokeWidth: 2
            PathLine {
                x: root.width
                y: endPath.startY
            }
        }
    }
}
