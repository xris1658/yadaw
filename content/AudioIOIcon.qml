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
            path: "M80,32L32,80H64v32H96V80h32Zm80,16V80H128l48,48,48-48H192V48H160ZM32,176s16-48,48-48,48,48,48,48,16,48,48,48,48-48,48-48H32Z"
        }
    }
}
