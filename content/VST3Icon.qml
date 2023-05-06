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
            path: "M112,48H224L144,208H32l16-32H96l16-32H64l16-32h48l16-32H96Z"
        }
    }
}
