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
        startX: 4
        startY: 28
        PathLine {
            x: 44
            y: 28
        }
        PathLine {
            x: 44
            y: 4
        }
        PathLine {
            x: 52
            y: 4
        }
        PathLine {
            x: 100
            y: 52
        }
        PathLine {
            x: 124
            y: 52
        }
        PathLine {
            x: 124
            y: 76
        }
        PathLine {
            x: 100
            y: 76
        }
        PathLine {
            x: 52
            y: 124
        }
        PathLine {
            x: 44
            y: 124
        }
        PathLine {
            x: 44
            y: 100
        }
        PathLine {
            x: 4
            y: 100
        }
        PathLine {
            x: 4
            y: 88
        }
        PathLine {
            x: 44
            y: 88
        }
        PathLine {
            x: 44
            y: 40
        }
        PathLine {
            x: 4
            y: 40
        }
    }
}