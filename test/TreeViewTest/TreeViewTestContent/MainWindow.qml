import QtQuick
import QtQuick.Controls
import QtQuick.Window

import Models

Window {
    id: root
    visible: true
    width: 500
    height: 500
    property alias model: treeModelToListModel.sourceModel
    property alias treeModelToListModel: treeModelToListModel
    color: "#202020"
    ListView {
        id: listView
        anchors.fill: parent
        model: TreeModelToListModel {
            id: treeModelToListModel
        }
        delegate: ItemDelegate {
            width: listView.width
            height: 25
            leftPadding: height * (tmtlm_indent + 1)
            text: ftm_name
            Item {
                width: height
                height: parent.height
                visible: tmtlm_has_children
                anchors.left: parent.left
                anchors.leftMargin: parent.leftPadding - height
                Label {
                    text: tmtlm_expanded? "\u25b3": "\u25bd"
                    anchors.centerIn: parent
                }
            }
            onClicked: {
                if(tmtlm_has_children) {
                    if(tmtlm_expanded) {
                        treeModelToListModel.collapse(index);
                    }
                    else {
                        treeModelToListModel.expand(index);
                    }
                }
            }
        }
    }
}