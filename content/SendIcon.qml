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
            path: "M96,32V64l48,48h32V80l48,48-48,48V144H128L96,112v64h32L80,224,32,176H64V32H96Z"
        }
    }
}
