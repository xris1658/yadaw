pragma Singleton

import QtQml
import QtQuick

QtObject {
    readonly property color background: "#202020"
    readonly property color controlBackground: "#333333"
    readonly property color controlBorder: "#666666"
    readonly property color disabledControlBorder: "#444444"
    readonly property color mouseOverControlBackground: "#505050"
    readonly property color pressedControlBackground: "#1A1A1A"
    readonly property color highlightControlBackground: "#606060"
    readonly property color content: "#FFFFFF"
    readonly property color secondaryContent: "#CCCCCC"
    readonly property color disabledContent: "#A0A0A0"
    readonly property color link: "#0097F3"
    readonly property color visitedLink: "#C080C0"
    readonly property color menuBarBackground: "#000000"
    readonly property color highlightMenuBarBackground: "#333333"
    readonly property color scrollBarBackground: controlBackground
    readonly property color scrollBarHandle: "#505050"
    readonly property color scrollBarHandleHighlight: "#606060"
    readonly property color scrollBarHandlePressed: "#202020"
    readonly property color progressBarRunningColor: "#00C030"
    readonly property color progressBarPausedColor: "#F0A000"
    readonly property color progressBarStoppedColor: "#F00000"

    readonly property color bigClockBackground: background
    readonly property color bigClockText: content
    readonly property color topBarBackground: "#606060"
    readonly property color playingIcon: "#00FF00"
    readonly property color recordIcon: "#FF6666"
    readonly property color recordingIcon: "#FF3333"
}
