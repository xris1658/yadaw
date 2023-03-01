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
        startY: 64
        PathQuad {
            x: 64
            y: 64
            controlX: 32
            controlY: -64
        }
        PathQuad {
            x: 128
            y: 64
            controlX: 96
            controlY: 192
        }
    }
}