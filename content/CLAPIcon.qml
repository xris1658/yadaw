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
        startY: 104
        PathLine {
            x: 112
            y: 152
        }
        PathArc {
            x: 88
            y: 128
            radiusX: 24
            radiusY: 24
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 88
            y: 96
        }
        PathArc {
            x: 144
            y: 152
            radiusX: 56
            radiusY: 56
            useLargeArc: true
            direction: PathArc.Counterclockwise
        }
        PathLine {
            x: 144
            y: 104
        }
        PathArc {
            x: 168
            y: 128
            radiusX: 24
            radiusY: 24
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 168
            y: 160
        }
        PathArc {
            x: 112
            y: 104
            radiusX: 56
            radiusY: 56
            useLargeArc: true
            direction: PathArc.Counterclockwise
        }
    }
}
