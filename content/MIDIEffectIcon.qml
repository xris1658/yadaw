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
    }
    ShapePath {
        startX: 64
        startY: 16
        fillColor: Colors.background
        strokeColor: "transparent"
        PathArc {
            x: 64
            y: 32
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 64
            y: 16
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
    }
    ShapePath {
        startX: 40
        startY: 24
        fillColor: Colors.background
        strokeColor: "transparent"
        PathArc {
            x: 40
            y: 40
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 40
            y: 24
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
    }
    ShapePath {
        startX: 24
        startY: 48
        fillColor: Colors.background
        strokeColor: "transparent"
        PathArc {
            x: 24
            y: 64
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 24
            y: 48
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
    }
    ShapePath {
        startX: 88
        startY: 24
        fillColor: Colors.background
        strokeColor: "transparent"
        PathArc {
            x: 88
            y: 40
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 88
            y: 24
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
    }
    ShapePath {
        startX: 104
        startY: 48
        fillColor: Colors.background
        strokeColor: "transparent"
        PathArc {
            x: 104
            y: 64
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathArc {
            x: 104
            y: 48
            radiusX: 8
            radiusY: 8
            useLargeArc: true
            direction: PathArc.Clockwise
        }
    }
}
