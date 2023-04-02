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
            path: "M176,32h48V224H176V32ZM32,112H96V64l64,64L96,192V144H32V112Z"
        }
    }
}
