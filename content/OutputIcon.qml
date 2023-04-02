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
            path: "M32,32H80V224H32V32Zm64,80h64V64l64,64-64,64V144H96V112Z"
        }
    }
}
