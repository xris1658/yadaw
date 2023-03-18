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
            path: "M80,80h96a16,16,0,0,1,16,16v96a16,16,0,0,1-16,16H80a16,16,0,0,1-16-16V96A16,16,0,0,1,80,80Zm0,16h96v96H80V96ZM32,48H224V64H32V48Z"
        }
    }
}
