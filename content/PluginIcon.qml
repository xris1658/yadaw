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
            path: "M80,32h32V64h32l48,48h32v32H192l-48,48H112v32H80V176H32V160H80V96H32V80H80V32Z"
        }
    }
}
