import QtQuick

MainWindow {
    canClose: true

    Item {
        Component.onCompleted: {
            Global.qtVersion = "6.4.2";
        }
    }
}
