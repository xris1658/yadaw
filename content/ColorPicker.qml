import QtQuick
import QtQuick.Controls as QC
import QtQuick.Shapes

Item {
    id: root
    width: 256
    height: 288
    property color color

    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        width: 20
        height: width
        color: root.color
    }
    MouseArea {
        width: 256
        height: 256
        acceptedButtons: Qt.LeftButton
        cursorShape: Qt.CrossCursor
        enabled: true
        hoverEnabled: true
        Shape {
            id: hue
            z: 1
            ShapePath {
                startX: 128
                startY: 0
                fillGradient: ConicalGradient {
                    angle: 360
                    centerX: 128
                    centerY: 128
                    GradientStop {
                        position: 0 / 6.0
                        color: Qt.rgba(slider.value, 0, 0, 1)
                    }
                    GradientStop {
                        position: 1 / 6.0
                        color: Qt.rgba(slider.value, slider.value, 0, 1)
                    }
                    GradientStop {
                        position: 2 / 6.0
                        color: Qt.rgba(0, slider.value, 0, 1)
                    }
                    GradientStop {
                        position: 0.5
                        color: Qt.rgba(0, slider.value, slider.value, 1)
                    }
                    GradientStop {
                        position: 4 / 6.0
                        color: Qt.rgba(0, 0, slider.value, 1)
                    }
                    GradientStop {
                        position: 5 / 6.0
                        color: Qt.rgba(slider.value, 0, slider.value, 1)
                    }
                    GradientStop {
                        position: 1
                        color: Qt.rgba(slider.value, 0, 0, 1)
                    }
                }
                PathArc {
                    radiusX: 128
                    radiusY: 128
                    x: 128
                    y: 256
                }
                PathArc {
                    radiusX: 128
                    radiusY: 128
                    x: 128
                    y: 0
                }
            }
        }
        Shape {
            id: lightness
            z: 2
            ShapePath {
                startX: 128
                startY: 0
                fillGradient: RadialGradient {
                    centerX: 128
                    centerY: 128
                    centerRadius: 128
                    focalX: centerX
                    focalY: centerY
                    GradientStop {
                        position: 0
                        color: /*"#FFFFFF"*/Qt.rgba(slider.value, slider.value, slider.value, 1)
                    }
                    GradientStop {
                        position: 1
                        color: /*"#00FFFFFF"*/Qt.rgba(slider.value, slider.value, slider.value, 0)
                    }
                }
                PathArc {
                    radiusX: 128
                    radiusY: 128
                    x: 128
                    y: 256
                }
                PathArc {
                    radiusX: 128
                    radiusY: 128
                    x: 128
                    y: 0
                }
            }
        }
        onPositionChanged: (mouse) => {
            if(pressed) {
                let pointX = mouseX - 128;
                let pointY = 128 - mouseY;
                let hueValue = Math.atan(pointY / pointX) * 180 / Math.PI;
                if(pointX < 0) {
                    hueValue = 180 + hueValue;
                }
                else if(pointY < 0) {
                    hueValue = 360 + hueValue;
                }
                hueValue /= 360.0;
                let distance = Math.hypot(pointX, pointY);
                let saturationValue = distance / 128.0;
                console.log("Hue:       ", hueValue);
                console.log("Saturation:", saturationValue);
                root.color = Qt.hsva(hueValue, saturationValue, slider.value, 1);
            }
        }
    }
    QC.Slider {
        id: slider
        anchors.bottom: parent.bottom
        width: parent.width
    }
}
