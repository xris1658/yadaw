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
            path: "M48,32L32,224H64V144h48V112H80V64h48V32H48Zm96,0V48l16,80-32,96h32l16-64v64h32V208l-16-80,32-96H192L176,96V32H144Z"
        }
    }
}
