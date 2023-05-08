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
            path: "M80,32h32V80l32,32h16V80l48,48-48,48V144H128l-16-16v48h32L96,224,48,176H80V32Z"
        }
    }
}
