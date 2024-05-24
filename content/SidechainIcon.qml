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
            path: "M128,128h96v96H128V128Zm0-64,48,48,48-48H192V32H160V64H128ZM112,176L64,128v32H32v32H64v32Z"
        }
    }
}
