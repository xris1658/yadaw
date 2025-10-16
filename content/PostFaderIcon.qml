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
            path: "M 80,32 l 32,0 l 0,32 l 16,0 l 0,80 l -16,0 l 0,16 l 48,0 l 0,-32 l 48,48 l -48,48 l 0,-32 l -48,0 l 0,32 l -32,0 l 0,-80 l -16,0 l 0,-80 l 16,0 l 0,-32 Z M 80,80 l 32,0 l 0,16 l -32,0 l 0,-16 Z M 80,112 l 32,0 l 0,16 l -32,0 l 0,-16 Z"
        }
    }
}
