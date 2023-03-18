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
            path: "M48,48H208a16,16,0,0,1,16,16V192a16,16,0,0,1-16,16H48a16,16,0,0,1-16-16V64A16,16,0,0,1,48,48Zm0,32H208V192H48V80Z"
        }
    }
}
