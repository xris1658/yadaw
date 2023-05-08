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
            path: "M176,32h48V224H176V32ZM48,64h64V32l48,48-48,48V96H48V64Zm96,96v32H80v32L32,176l48-48v32h64Z"
        }
    }
}
