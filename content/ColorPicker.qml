import QtQml
import QtQuick
import QtQuick.Shapes

Item {
    id: root

    property int radius: 128
    property color color: "#000000"
    property alias spacing: column.spacing

    signal setColorOutside(colorOutside: color)

    width: radius * 2
    height: column.height

    QtObject {
        id: impl
        function fromColor(color: color) {
            let hue = color.hsvHue * 2 * Math.PI;
            let saturation = color.hsvSaturation;
            let value = color.hsvValue;
            let x = root.radius + root.radius * Math.cos(hue) * saturation;
            let y = root.radius - root.radius * Math.sin(hue) * saturation;
            locator.centerX = x;
            locator.centerY = y;
            slider.value = value;
        }
        function toColor() {
            let pointX = locator.x + Math.floor(locator.width / 2) - root.radius;
            let pointY = root.radius - locator.y - Math.floor(locator.height / 2);
            let hueValue = Math.atan(pointY / pointX) / Math.PI;
            if(pointX < 0) {
                hueValue = 1 + hueValue;
            }
            else if(pointY < 0) {
                hueValue = 2 + hueValue;
            }
            hueValue /= 2.0;
            let distance = Math.hypot(pointX, pointY);
            let saturationValue = distance / radius;
            root.color = Qt.hsva(hueValue, saturationValue, slider.value, 1);
        }
    }

    Column {
        id: column
        MouseArea {
            width: radius * 2
            height: radius * 2
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.CrossCursor
            enabled: true
            hoverEnabled: true
            clip: true
            Rectangle {
                property int centerX: root.radius + Math.cos(root.color.hsvHue * 2 * Math.PI) * root.radius * root.color.hsvSaturation
                property int centerY: -1 * Math.sin(root.color.hsvHue * 2 * Math.PI) * root.radius * root.color.hsvSaturation + root.radius
                z: 3
                id: locator
                width: 11
                height: width
                radius: width / 2
                color: "transparent"
                border.width: 2
                border.color: root.color.hsvValue > 0.5? "#000000": "#FFFFFF"
                x: centerX - Math.floor(width / 2)
                y: centerY - Math.floor(height / 2)
            }
            Shape {
                id: hue
                z: 1
                ShapePath {
                    startX: radius
                    startY: 0
                    fillGradient: ConicalGradient {
                        angle: 360
                        centerX: radius
                        centerY: radius
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
                        radiusX: radius
                        radiusY: radius
                        x: radius
                        y: radius * 2
                    }
                    PathArc {
                        radiusX: radius
                        radiusY: radius
                        x: radius
                        y: 0
                    }
                }
            }
            Shape {
                id: lightness
                z: 2
                ShapePath {
                    startX: radius
                    startY: 0
                    fillGradient: RadialGradient {
                        centerX: radius
                        centerY: radius
                        centerRadius: radius
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
                        radiusX: radius
                        radiusY: radius
                        x: radius
                        y: radius * 2
                    }
                    PathArc {
                        radiusX: radius
                        radiusY: radius
                        x: radius
                        y: 0
                    }
                }
            }
            onPositionChanged: (mouse) => {
                if(pressed) {
                    let locatorX = mouseX - radius;
                    let locatorY = mouseY - radius;
                    let hypot = Math.hypot(locatorX, locatorY);
                    if(hypot > radius) {
                        locatorX *= (radius / hypot);
                        locatorY *= (radius / hypot);
                    }
                    locator.centerX = locatorX + radius;
                    locator.centerY = locatorY + radius;
                    impl.toColor();
                }
            }
            onPressed: (mouse) => {
                onPositionChanged(mouse);
            }
        }
        Slider {
            id: slider
            width: root.width
            live: true
            groove.gradient: Gradient {
                orientation: Qt.Horizontal
                GradientStop {
                    position: 0.0
                    color: "#000000"
                }
                GradientStop {
                    position: 1.0
                    color: "#FFFFFF"
                }
            }
            onValueChanged: {
                impl.toColor();
            }
        }
    }
    onSetColorOutside: (colorOutside) => {
        impl.fromColor(colorOutside)
    }
}
