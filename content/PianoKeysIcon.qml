import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 256
    readonly property int originalHeight: 256
    width: originalWidth
    height: originalHeight
    property alias path: path1
    ShapePath {
        id: path1
        startX: 16
        startY: 32
        PathLine {
            x: 64
            y: 32
        }
        PathLine {
            x: 64
            y: 144
        }
        PathLine {
            x: 80
            y: 144
        }
        PathLine {
            x: 80
            y: 224
        }
        PathLine {
            x: 16
            y: 224
        }
    }
    ShapePath {
        id: path2
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 112
        startY: 32
        PathLine {
            x: 144
            y: 32
        }
        PathLine {
            x: 144
            y: 144
        }
        PathLine {
            x: 160
            y: 144
        }
        PathLine {
            x: 160
            y: 224
        }
        PathLine {
            x: 96
            y: 224
        }
        PathLine {
            x: 96
            y: 144
        }
        PathLine {
            x: 112
            y: 144
        }
    }
    ShapePath {
        id: path3
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 192
        startY: 32
        PathLine {
            x: 240
            y: 32
        }
        PathLine {
            x: 240
            y: 224
        }
        PathLine {
            x: 176
            y: 224
        }
        PathLine {
            x: 176
            y: 144
        }
        PathLine {
            x: 192
            y: 144
        }
    }
}
