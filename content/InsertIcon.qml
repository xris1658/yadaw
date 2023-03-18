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
            path: "M80,64h96a16,16,0,0,1,16,16v96a16,16,0,0,1-16,16H80a16,16,0,0,1-16-16V80A16,16,0,0,1,80,64Zm0,16h96v96H80V80ZM32,120H64v16H32V120Zm160,0h32v16H192V120ZM80"
        }
    }
}
