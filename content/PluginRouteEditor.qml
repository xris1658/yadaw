import QtQuick
import QtQuick.Layouts

import YADAW.Models

Item {
    id: root

    property alias inputListModel: inputRouteList.inputListModel
    property alias outputListModel: outputRouteList.outputListModel
    property alias inputRouteListModel: inputRouteList.model
    property alias outputRouteListModel: outputRouteList.model
    property Window audioIOSelectorWindow

    onInputListModelChanged: {
        if(inputListModel) {
            if(!priv.modelDesctructionDetector) {
                // Handle `QObject::destroyed` by creating a child of that
                // object so that `Component.destruction` is emitted when its
                // parent is destroyed
                // TODO: Move the following QML code into a new file if it is reused
                priv.modelDesctructionDetector = Qt.createQmlObject(`
                    import QtQml
                    
                    QtObject {
                        signal parentDestructed()
                        Component.onDestruction: {
                            parentDestructed();
                        }
                    }`,
                    inputListModel
                );
            }
            priv.modelDesctructionDetector.parent = inputListModel;
        }
    }
    QtObject {
        id: priv
        property QtObject modelDesctructionDetector: null
    }
    Connections {
        target: priv.modelDesctructionDetector
        function onParentDestructed() {
            inputListModel = null;
            outputListModel = null;
            inputRouteListModel = null;
            outputRouteListModel = null;
            priv.modelDesctructionDetector = null;
        }
    }
    Column {
        id: column
        anchors.centerIn: parent
        TabBar {
            id: tabButtons
            TabButton {
                id: inputButton
                width: implicitWidth
                text: qsTr("Inputs")
                checked: true
            }
            TabButton {
                id: outputButton
                width: implicitWidth
                text: qsTr("Outputs")
            }
        }
        Rectangle {
            id: tabContent
            width: root.width
            height: root.height - tabButtons.height
            color: "transparent"
            border.color: Colors.border
            StackLayout {
                id: stackLayout
                anchors.fill: parent
                anchors.margins: parent.border.width
                currentIndex: tabButtons.currentIndex
                InputRouteList {
                    id: inputRouteList
                    width: stackLayout.width
                    height: stackLayout.height
                }
                OutputRouteList {
                    id: outputRouteList
                    width: stackLayout.width
                    height: stackLayout.height
                }
            }
        }
    }
}
