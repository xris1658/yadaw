pragma Singleton

import QtQuick

QtObject {
    property int qtMajorVersion: 0
    property int qtMinorVersion: 0
    property int qtMicroVersion: 0
    property string qtCopyrightYear: "Unknown"
    property string fileBrowserName: qsTr("File Browser")
    function messageDialog(message: string, title: string, icon: int, modal: bool) {
        let component = Qt.createComponent("./MessageDialog.qml");
        if(component.status === Component.Ready) {
            let dialog = component.createObject(null);
            dialog.message = message;
            dialog.title = title;
            dialog.icon = icon;
            if(modal) {
                dialog.modality = Qt.NonModal;
            }
            dialog.showNormal();
        }
        else {
            console.log("messageDialog() failed with error:", component.errorString());
        }
    }
}
