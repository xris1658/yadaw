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
            path: "M64,32h80V48H112V208h32v16H64V208H96V48H64V32ZM48,80H80V96H64v64H80v16H48V80Zm80,0V96h64v64H128v16h80V80H128Z"
        }
    }
}
