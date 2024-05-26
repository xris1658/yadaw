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
            path: "M80,32L32,80H64v32H96V80h32Zm80,16V80H128l48,48,48-48H192V48H160ZM32,128v96H80V176H64V128H32Zm80,0v48H96v48h64V176H144V128H112Zm80,0v48H176v48h48V128H192Z"
        }
    }
}
