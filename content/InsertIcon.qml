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
            path: "M64,64H192v48h32v32H192v48H64V144H32V112H64V64ZM80,80h96v32H80V80Zm0,64h96v32H80V144Z"
        }
    }
}
