pragma Singleton

import QtQml
import QtQuick

QtObject {
    readonly property color background: "#202020"
    readonly property color secondaryBackground: "#131313"
    readonly property color controlBackground: "#333333"
    readonly property color border: "#666666"
    readonly property color secondaryBorder: "#444444"
    readonly property color unacceptableBorder: "#FF6666"
    readonly property color controlBorder: border
    readonly property color disabledControlBorder: secondaryBorder
    readonly property color mouseOverControlBackground: "#424242"
    readonly property color pressedControlBackground: "#1A1A1A"
    readonly property color highlightControlBackground: "#606060"
    readonly property color content: "#FFFFFF"
    readonly property color secondaryContent: "#CCCCCC"
    readonly property color disabledContent: "#A0A0A0"
    readonly property color link: "#0080FF"
    readonly property color visitedLink: "#C080C0"
    readonly property color menuBarBackground: "#000000"
    readonly property color highlightMenuBarBackground: "#333333"
    readonly property color scrollBarBackground: controlBackground
    readonly property color scrollBarHandle: "#505050"
    readonly property color scrollBarHandleHighlight: Colors.highlightControlBackground
    readonly property color scrollBarHandlePressed: "#202020"
    readonly property color progressBarRunningColor: "#00C030"
    readonly property color progressBarPausedColor: "#F0A000"
    readonly property color progressBarStoppedColor: "#F00000"
    readonly property color checkedButtonBackground: "#DDDDDD"
    readonly property color mouseOverCheckedButtonBackground: "#CCCCCC"
    readonly property color pressedCheckedButtonBackground: "#AAAAAA"
    readonly property color checkedButtonContent: "#202020"
    readonly property color highlightListViewItemBackground: "#3050FF"
    readonly property color sliderWithValueFilledBackground: "#3050FF"

    readonly property color mutedButtonBackground: "#FFC000"
    readonly property color soloedButtonBackground: "#00A0FF"
    readonly property color invertedButtonBackground: "#40C000"
    readonly property color monitorButtonBackground: "#C6F000"
    readonly property color armRecordingButtonBackground: "#F02000"

    readonly property color bigClockBackground: background
    readonly property color bigClockText: content
    readonly property color topBarBackground: "#606060"
    readonly property color playingIcon: "#00FF00"
    readonly property color recordIcon: "#FF6666"
    readonly property color recordingIcon: "#FF3333"

    readonly property color automationColor: "#D06040"

    function isBright(color) {
        return color.r * 0.299 + color.g * 0.587 + color.b * 0.514 > 0.6;
    }
}
