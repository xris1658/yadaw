import QtQuick
import Qt.labs.folderlistmodel

QtObject {
    id: loader
    property url fontDirectory: Qt.resolvedUrl("./fonts")

    function loadFont(url) {
        console.log(url);
        var fontLoader = Qt.createQmlObject('import QtQuick; FontLoader { source: "' + url + '"; }',
                                            loader,
                                            "dynamicFontLoader");
    }

    property FolderListModel folderModel: FolderListModel {
        id: folderModel
        folder: loader.fontDirectory
        nameFilters: [ "*.ttf", "*.otf" ]
        showDirs: false

        onStatusChanged: {
            if (folderModel.status == FolderListModel.Ready) {
                var i
                for (i = 0; i < count; i++) {
                    loadFont(folderModel.get(i, "fileURL"))
                }
            }
        }
    }
}