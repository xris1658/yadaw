import QtQml
import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property int firstColumnWidth
    property int secondColumnWidth

    property alias audioGraphInputDeviceList: audioGraphSettings.inputDeviceList
    property alias audioGraphOutputDeviceList: audioGraphSettings.outputDeviceList

    Grid {
        id: grid
        columns: 2
        columnSpacing: 10
        rowSpacing: 5
        verticalItemAlignment: Grid.AlignVCenter
        horizontalItemAlignment: Grid.AlignHCenter
        Label {
            width: firstColumnWidth
            text: qsTr("Audio Engine")
            color: Colors.secondaryContent
        }
        Item {
            width: secondColumnWidth
            height: 5
        }
        Label {
            width: firstColumnWidth
            text: qsTr("Audio Backend")
            horizontalAlignment: Label.AlignRight
        }
        ComboBox {
            id: audioEngineSelector
            width: secondColumnWidth
            model: [qsTr("Dummy"), "AudioGraph"]
        }
    }
    StackLayout {
        anchors.top: grid.bottom
        anchors.topMargin: grid.rowSpacing
        currentIndex: audioEngineSelector.currentIndex
        Item {}
        AudioGraphSettings {
            id: audioGraphSettings
            firstColumnWidth: root.firstColumnWidth
            secondColumnWidth: root.secondColumnWidth
        }
    }
}
