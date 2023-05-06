import QtQuick
import QtQuick.Shapes

Shape {
    readonly property int originalWidth: 16
    readonly property int originalHeight: 16
    width: originalWidth
    height: originalHeight
    property alias path: path
    ShapePath {
        id: path
        fillRule: ShapePath.OddEvenFill
        PathSvg {
            path: "M4,2H9V3H7V13H9v1H4V13H6V3H4V2ZM5,5V6H4v4H5v1H3V5H5ZM8,5h5v6H8V10h4V6H8V5Z"
        }
    }
}
