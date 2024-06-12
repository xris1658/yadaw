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
        fillRule: ShapePath.WindingFill
        PathSvg {
            path: "M 112,32 h 32 V 64 L 128,96 L 112,64 Z
                   M 144,224 h -32 v -32 l 16,-32 l 16,32 Z
                   M 32,144 v -32 h 32 l 32,16 l -32,16 Z
                   M 224,112 v 32 h -32 l -32,-16 l 32,-16 Z
                   M 128,112 A 16,16, 0,0,1 128,144 A 16,16, 0,0,1 128,112 Z
                   M 128,48 a 80,80, 0,0,1 0,160 a 80,80, 0,0,1 0,-160
                   m 0,16 a 64,64, 0,0,0 0,128 a 64,64, 0,0,0 0,-128
                   m 0,-16z
                   "
        }
    }
}
