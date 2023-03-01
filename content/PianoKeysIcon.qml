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
        startX: 4
        startY: 16
        PathLine {
            x: 28
            y: 16
        }
        PathLine {
            x: 28
            y: 80
        }
        PathLine {
            x: 36
            y: 80
        }
        PathLine {
            x: 36
            y: 118
        }
        PathLine {
            x: 4
            y: 118
        }
    }
    ShapePath {
        id: path2
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 52
        startY: 16
        PathLine {
            x: 76
            y: 16
        }
        PathLine {
            x: 76
            y: 80
        }
        PathLine {
            x: 82
            y: 80
        }
        PathLine {
            x: 82
            y: 118
        }
        PathLine {
            x: 46
            y: 118
        }
        PathLine {
            x: 46
            y: 80
        }
        PathLine {
            x: 52
            y: 80
        }
    }
    ShapePath {
        id: path3
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 100
        startY: 16
        PathLine {
            x: 124
            y: 16
        }
        PathLine {
            x: 124
            y: 118
        }
        PathLine {
            x: 92
            y: 118
        }
        PathLine {
            x: 92
            y: 80
        }
        PathLine {
            x: 100
            y: 80
        }
    }
}
