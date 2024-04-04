import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Models 1.0

Window {
    id: root
    visible: true
    width: 500
    height: 500
    color: "#202020"

    property IntegerModel sourceModel
    Row {
        ListView {
            id: listView
            model: root.sourceModel
            width: 250
            height: 500
            delegate: ItemDelegate {
                width: listView.width
                text: im_value
            }
        }
        ListView {
            id: proxyView
            model: SortFilterProxyListModel {
                sourceModel: listView.model
            }
            width: 250
            height: 500
            delegate: ItemDelegate {
                width: proxyView.width
                text: im_value
            }
            Component.onCompleted: {
                model.appendSortOrder(IntegerModel.Value, Qt.AscendingOrder);
            }
        }
    }
}