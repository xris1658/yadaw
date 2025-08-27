import QtQuick

import YADAW.Models

Item {
    id: root
    property alias model: proxyModel.sourceModel
    property ListView listView: listView

    TreeModelToListModel {
        id: proxyModel
    }
    ListView {
        id: listView
        property alias treeView: root
        anchors.fill: parent
        model: proxyModel
    }
    function expand(destIndex: int) {
        proxyModel.expand(destIndex);
    }
    function expandRecursively(destIndex: int) {
        proxyModel.expandRecursively(destIndex);
    }
    function collapse(destIndex: int) {
        proxyModel.collapse(destIndex);
    }
    function collapseRecursively(destIndex: int) {
        proxyModel.collapseRecursively(destIndex);
    }
    function toggleExpanded(destIndex: int) {
        proxyModel.toggleExpanded(destIndex);
    }
    function expandToIndex(sourceIndex): int {
        return proxyModel.expandToIndex(sourceIndex);
    }
}