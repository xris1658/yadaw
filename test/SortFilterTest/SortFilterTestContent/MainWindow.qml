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
    readonly property SortFilterProxyListModel proxyModel: sortFilterProxyListModel
    Row {
        ListView {
            id: listView
            model: root.sourceModel
            width: 250
            height: 500
            header: ItemDelegate {
                width: listView.width
                text: "Original"
            }
            delegate: ItemDelegate {
                width: listView.width
                text: im_value
            }
        }
        ListView {
            id: proxyView
            model: SortFilterProxyListModel {
                id: sortFilterProxyListModel
                sourceModel: listView.model
            }
            width: 250
            height: 500
            delegate: ItemDelegate {
                width: proxyView.width
                text: im_value
            }
            header: ItemDelegate {
                text: "Toggle sort order"
                width: proxyView.width
                property bool sortOn: true;
                property int sortOrder: Qt.AscendingOrder;
                Label {
                    id: indicator
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: ""
                    anchors.rightMargin: parent.rightPadding
                }
                onClicked: {
                    proxyView.model.removeSortOrder(0);
                    if(sortOn) {
                        proxyView.model.appendSortOrder(IntegerModel.Value, sortOrder);
                        if(sortOrder == Qt.AscendingOrder) {
                            indicator.text = "^";
                            sortOrder = Qt.DescendingOrder;
                        }
                        else {
                            indicator.text = "v";
                            sortOrder = Qt.AscendingOrder;
                            sortOn = false;
                        }
                    }
                    else {
                        indicator.text = "";
                        sortOn = true;
                    }
                }
            }
            Component.onCompleted: {
                model.setValueOfFilter(IntegerModel.Value, 12);
            }
        }
    }
}