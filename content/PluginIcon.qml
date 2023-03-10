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
        startX: 80
        startY: 32
        PathLine {
            x: 96
            y: 32
        }
        PathLine {
            x: 176
            y: 112
        }
        PathLine {
            x: 224
            y: 112
        }
        PathLine {
            x: 224
            y: 144
        }
        PathLine {
            x: 176
            y: 144
        }
        PathLine {
            x: 96
            y: 224
        }
        PathLine {
            x: 80
            y: 224
        }
        PathLine {
            x: 80
            y: 176
        }
        PathLine {
            x: 32
            y: 176
        }
        PathLine {
            x: 32
            y: 160
        }
        PathLine {
            x: 80
            y: 160
        }
        PathLine {
            x: 80
            y: 96
        }
        PathLine {
            x: 32
            y: 96
        }
        PathLine {
            x: 32
            y: 80
        }
        PathLine {
            x: 80
            y: 80
        }
    }
}
