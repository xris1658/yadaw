import QtQuick

MainWindow {
    id: mainWindow
    translationModel: ListModel {
        ListElement {
            lm_name: "English"
        }
        ListElement {
            lm_name: "Simplified Chinese"
        }
        ListElement {
            lm_name: "Loooooooooooooooooooooooooooooooooooooooooooooooong text"
        }
    }

    canClose: true

    Item {
        Component.onCompleted: {
            Global.qtVersion = "6.5.0";
        }
    }
}
