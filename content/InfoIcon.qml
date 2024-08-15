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
            path: "m 144,96 V 208 H 112 V 112 H 96 V 96
                   M 112 64 a 16,16 0 0 1 32,0 a 16,16 0 0 1 -32,0 Z"
        }
    }
}
