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
        startX: 28
        startY: 112
        PathArc {
            x: 100
            y: 112
            radiusX: 60
            radiusY: 60
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 88
            y: 80
        }
        PathLine {
            x: 40
            y: 80
        }
        PathLine {
            x: 28
            y: 112
        }
        PathLine {
            x: 24
            y: 64
        }
        // 1
        PathArc {
            x: 24
            y: 48
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 24
            y: 64
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 40
            y: 64
        }
        PathLine {
            x: 40
            y: 40
        }
        // 2
        PathArc {
            x: 40
            y: 24
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 40
            y: 40
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 64
            y: 40
        }
        PathLine {
            x: 64
            y: 32
        }
        PathArc {
            x: 64
            y: 16
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 64
            y: 32
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 64
            y: 40
        }
        PathLine {
            x: 88
            y: 40
        }
        PathArc {
            x: 88
            y: 24
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 88
            y: 40
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 88
            y: 64
        }
        PathLine {
            x: 104
            y: 64
        }
        PathArc {
            x: 104
            y: 48
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 104
            y: 64
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 88
            y: 64
        }
        PathLine {
            x: 88
            y: 40
        }
        PathLine {
            x: 40
            y: 40
        }
        PathLine {
            x: 40
            y: 64
        }
        PathLine {
            x: 24
            y: 64
        }
    }
}
