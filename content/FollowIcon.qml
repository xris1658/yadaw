import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 128
    readonly property int originalHeight: 128
    width: originalWidth
    height: originalHeight
    property alias path: path1
    ShapePath {
        id: path1
        startX: 28
        startY: 0
        PathLine {
            x: 92
            y: 0
        }
        PathLine {
            x: 92
            y: 32
        }
        PathLine {
            x: 64
            y: 60
        }
        PathLine {
            x: 64
            y: 128
        }
        PathLine {
            x: 56
            y: 128
        }
        PathLine {
            x: 56
            y: 60
        }
        PathLine {
            x: 28
            y: 32
        }
    }
    ShapePath {
        id: path2
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 0
        startY: 76
        PathLine {
            x: 52
            y: 76
        }
        PathLine {
            x: 52
            y: 92
        }
        PathLine {
            x: 0
            y: 92
        }
    }
    ShapePath {
        id: path3
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 68
        startY: 76
        PathLine {
            x: 96
            y: 76
        }
        PathLine {
            x: 96
            y: 52
        }
        PathLine {
            x: 128
            y: 84
        }
        PathLine {
            x: 96
            y: 116
        }
        PathLine {
            x: 96
            y: 92
        }
        PathLine {
            x: 68
            y: 92
        }
    }
}
