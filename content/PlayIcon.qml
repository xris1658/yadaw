import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 60
    readonly property int originalHeight: 70
    width: originalWidth
    height: originalHeight
    property alias path: path
    ShapePath {
        id: path
        startX: 0
        startY: 0
        PathLine {
            x: 60
            y: 35
        }
        PathLine {
            x: 0
            y: 70
        }
    }
}
