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
        fillRule: ShapePath.OddEvenFill
        PathSvg {
            path: "M192,32l32,32-64,64,64,64-32,32-64-64L64,224,32,192l64-64L32,64,64,32l64,64Z"
        }
    }
}
