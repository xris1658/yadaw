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
        fillRule: ShapePath.WindingFill
        PathSvg {
            path: "M48,192l16,16H80V112A 48,48, 0,0,1 176,112v96h16l16-16V144l-16-16V112A64,64 1,1,0 64,112v16L48,144v48Z"
        }
    }
}
