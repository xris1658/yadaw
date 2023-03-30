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
    }

    canClose: true

    Item {
        Component.onCompleted: {
            Global.qtVersion = "6.5.0";
        }
    }
}
