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
            path: "M112,32h32V48H112V32Zm0,128h32v64H112V160ZM96,64h64v80H96V64Zm16,16h32V96H112V80Zm0,32h32v16H112V112Z"
        }
    }
}
