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
        startX: 112
        startY: 48
        PathLine {
            x: 224
            y: 48
        }
        PathLine {
            x: 144
            y: 208
        }
        PathLine {
            x: 32
            y: 208
        }
        PathLine {
            x: 48
            y: 176
        }
        PathLine {
            x: 96
            y: 176
        }
        PathLine {
            x: 112
            y: 144
        }
        PathLine {
            x: 64
            y: 144
        }
        PathLine {
            x: 80
            y: 112
        }
        PathLine {
            x: 128
            y: 112
        }
        PathLine {
            x: 144
            y: 80
        }
        PathLine {
            x: 96
            y: 80
        }
        PathLine {
            x: 112
            y: 48
        }
    }
}
