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
        PathSvg {
            path: "M 112,88 A 24,24 0 0 1 88,112 24,24 0 0 1 64,88 24,24 0 0 1 88,64 24,24 0 0 1 112,88 Z M 32,32 h 96 l 96,96 -96,96 -96,-96 z"
        }
    }
}
