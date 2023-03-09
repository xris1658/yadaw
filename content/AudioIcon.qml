import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 256
    readonly property int originalHeight: 256
    width: originalWidth
    height: originalHeight
    property alias path: path
    ShapePath {
        id: path
        startX: 32
        startY: 128
        PathQuad {
            x: 128
            y: 128
            controlX: 80
            controlY: -64
        }
        PathQuad {
            x: 224
            y: 128
            controlX: 176
            controlY: 320
        }
    }
}
