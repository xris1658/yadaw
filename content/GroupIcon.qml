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
            path: "M32,32h80V80H32V32ZM144,176h80v48H144V176Zm0-64h80v48H144V112ZM64,96H80v32h48v16H80v48h48v16H64V96Z"
        }
    }
}
