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
            path: "M32,32h96v96H32V32Zm0,144,48,48,48-48H96V144H64v32H32ZM224,80L176,32V64H144V96h32v32Z"
        }
    }
}
