pragma Singleton

import QtQuick
import QtQuick.Controls as QC

QtObject {
    property string qtVersion: "Unknown"
    function messageDialog(message: string, title: string, icon: int, modal: bool) {
        let component = Qt.createComponent("./MessageDialog.qml");
        if(component.status === Component.Ready) {
            let dialog = component.createObject(null);
            dialog.message = message;
            dialog.title = title;
            dialog.standardButtons = QC.DialogButtonBox.Ok;
            dialog.icon = icon;
            if(modal) {
                dialog.modality = Qt.NonModal;
            }
            dialog.showNormal();
        }
        else {
            console.log(component.errorString());
        }
    }
}
