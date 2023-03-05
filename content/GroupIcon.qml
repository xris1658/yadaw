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
        startX: 16
        startY: 24
        PathLine {
            x: 24
            y: 24
        }
        PathLine {
            x: 24
            y: 32
        }
        PathLine {
            x: 16
            y: 32
        }
    }
    ShapePath {
        startX: 28
        startY: 48
        PathArc {
            x: 28
            y: 56
            radiusX: 4
            radiusY: 4
        }
        PathArc {
            x: 28
            y: 48
            radiusX: 4
            radiusY: 4
        }
    }
    ShapePath {
        startX: 16
        startY: 72
        PathLine {
            x: 24
            y: 72
        }
        PathLine {
            x: 24
            y: 80
        }
        PathLine {
            x: 16
            y: 80
        }
    }
    ShapePath {
        startX: 28
        startY: 96
        PathArc {
            x: 28
            y: 104
            radiusX: 4
            radiusY: 4
        }
        PathArc {
            x: 28
            y: 96
            radiusX: 4
            radiusY: 4
        }
    }
    ShapePath {
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 32
        startY: 24
        PathLine {
            x: 112
            y: 24
        }
        PathLine {
            x: 112
            y: 32
        }
        PathLine {
            x: 32
            y: 32
        }
    }
    ShapePath {
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 40
        startY: 48
        PathLine {
            x: 112
            y: 48
        }
        PathLine {
            x: 112
            y: 56
        }
        PathLine {
            x: 40
            y: 56
        }
    }
    ShapePath {
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 32
        startY: 72
        PathLine {
            x: 112
            y: 72
        }
        PathLine {
            x: 112
            y: 80
        }
        PathLine {
            x: 32
            y: 80
        }
    }
    ShapePath {
        fillColor: path1.fillColor
        strokeColor: path1.strokeColor
        strokeWidth: path1.strokeWidth
        startX: 40
        startY: 96
        PathLine {
            x: 112
            y: 96
        }
        PathLine {
            x: 112
            y: 104
        }
        PathLine {
            x: 40
            y: 104
        }
    }
}