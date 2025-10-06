import QtQuick

import YADAW.Models

Item {
    id: root

    property alias inputListModel: inputRouteList.ioListModel
    property alias outputListModel: outputRouteList.ioListModel
    property alias inputRouteListModel: inputRouteList.model
    property alias outputRouteListModel: outputRouteList.model
    property Window audioIOSelectorWindow

    Item {
        id: splitter
        anchors.horizontalCenter: parent.horizontalCenter
        width: 1
        height: root.height
    }
    Item {
        id: auxInputHeader
        height: auxInputLabel.height
        Label {
            id: auxInputLabel
            text: qsTr("Inputs")
            leftPadding: height + auxInputIconPlaceholder.anchors.leftMargin
            bottomPadding: 3
            Item {
                id: auxInputIconPlaceholder
                width: parent.height - parent.bottomPadding
                height: width
                anchors.left: parent.left; anchors.leftMargin: 3
                AuxInputIcon {
                    anchors.centerIn: parent
                    scale: 16 / originalHeight
                    path.strokeColor: "transparent"
                    path.fillColor: Colors.secondaryContent
                }
            }
        }
    }
    Item {
        id: auxOutputHeader
        anchors.left: outputRouteListPlaceholder.left
        height: auxOutputLabel.height
        Label {
            id: auxOutputLabel
            text: qsTr("Outputs")
            leftPadding: height + auxOutputIconPlaceholder.anchors.leftMargin
            bottomPadding: 3
            Item {
                id: auxOutputIconPlaceholder
                width: parent.height - parent.bottomPadding
                height: width
                anchors.left: parent.left; anchors.leftMargin: 3
                AuxOutputIcon {
                    anchors.centerIn: parent
                    scale: 16 / originalHeight
                    path.strokeColor: "transparent"
                    path.fillColor: Colors.secondaryContent
                }
            }
        }
    }
    Rectangle {
        id: inputRouteListPlaceholder
        color: "transparent"
        border.width: 1
        border.color: Colors.border
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: splitter.left
        anchors.leftMargin: 0
        anchors.topMargin: auxInputHeader.height
        anchors.bottomMargin: 0
        anchors.rightMargin: -1 * splitter.width
        RouteList {
            id: inputRouteList
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: parent.border.width
            anchors.bottomMargin: parent.border.width
            audioIOSelectorWindow: root.audioIOSelectorWindow
            isInput: true
        }
    }
    Rectangle {
        id: outputRouteListPlaceholder
        color: "transparent"
        border.color: Colors.border
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: splitter.right
        anchors.rightMargin: 0
        anchors.topMargin: auxOutputHeader.height
        anchors.bottomMargin: 0
        anchors.leftMargin: -1 * splitter.width
        RouteList {
            id: outputRouteList
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: parent.border.width
            anchors.bottomMargin: parent.border.width
            audioIOSelectorWindow: root.audioIOSelectorWindow
            isInput: false
        }
    }
}