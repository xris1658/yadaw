import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 100
    readonly property int actualHeight: 120
    readonly property int originalHeight: 140
    width: originalWidth
    height: originalHeight
    property alias path: path
    ShapePath {
        id: path
        startX: 80
        startY: 30
        PathLine {
            x: 50
            y: 60
        }
        PathLine {
            x: 50
            y: 40
        }
        PathArc {
            x: 80
            y: 70
            radiusX: 30
            radiusY: radiusX
            useLargeArc: true
            direction: PathArc.Counterclockwise
        }
        PathLine {
            x: 100
            y: 70
        }
        PathArc {
            x: 50
            y: 20
            radiusX: 50
            radiusY: radiusX
            useLargeArc: true
            direction: PathArc.Clockwise
        }
        PathLine {
            x: 50
            y: 00
        }
        PathLine {
            x: 80
            y: 30
        }
    }
}
