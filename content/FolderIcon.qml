import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 128
    readonly property int originalHeight: 128
    width: originalWidth
    height: originalHeight
    property alias path: path
    ShapePath {
        id: path
        startX: 0
        startY: 4
        PathLine {
            x: 48
            y: 4
        }
        PathLine {
            x: 48
            y: 24
        }
        PathLine {
            x: 128
            y: 24
        }
        PathLine {
            x: 128
            y: 112
        }
        PathLine {
            x: 0
            y: 112
        }
    }
}
