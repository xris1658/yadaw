import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 160
    readonly property int originalHeight: 160
    width: originalWidth
    height: originalHeight
    property alias path: path
    ShapePath {
        id: path
        fillRule: ShapePath.OddEvenFill
        startX: 63
        startY: 0
        PathLine {
            x: 95
            y: 0
        }
        PathLine {
            x: 135
            y: 160
        }
        PathLine {
            x: 99
            y: 160
        }
        PathLine {
            x: 160
            y: 23
        }
        PathLine {
            x: 134
            y: 10
        }
        PathLine {
            x: 59
            y: 160
        }
        PathLine {
            x: 25
            y: 160
        }
    }
}
