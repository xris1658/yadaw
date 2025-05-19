import QtQuick
import QtQuick.Controls as QC

import YADAW.Models

QC.Popup {
    id: root

    signal accepted()
    signal tryDefault()
    signal cancelled()

    property alias audioBusConfigurationListProxyModel: audioBusConfigurationListProxyModel

    QtObject {
        id: impl
        property int contentWidth: 350
    }

    SortFilterProxyListModel {
        id: audioBusConfigurationListProxyModel
    }

    background: Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.topInset
        anchors.bottomMargin: root.bottomInset
        anchors.leftMargin: root.leftInset
        anchors.rightMargin: root.rightInset
        color: Colors.background
        border.color: Colors.border
    }

    contentItem: Column {
        spacing: 5
        Rectangle {
            width: impl.contentWidth
            height: 300
            color: "transparent"
            border.color: Colors.border
            TableLikeListView {
                id: audioChannelConfigList
                anchors.fill: parent
                anchors.margins: parent.border.width
                listView.activeFocusOnTab: true
                listView.flickableDirection: Flickable.AutoFlickDirection
                listView.boundsBehavior: Flickable.StopAtBounds
                listView.reuseItems: true
                headerListModel: ListModel {
                    dynamicRoles: true
                    Component.onCompleted: {
                        append({
                            "title": qsTr("Name"),
                            "field": "clapabclm_name",
                            "columnWidth": 98,
                            "roleId": ICLAPAudioBusConfigurationListModel.Name
                        });
                        append({
                            "title": qsTr("In Cnt."),
                            "field": "clapabclm_input_count",
                            "columnWidth": 50,
                            "roleId": ICLAPAudioBusConfigurationListModel.InputCount
                        });
                        append({
                            "title": qsTr("Out Cnt."),
                            "field": "clapabclm_output_count",
                            "columnWidth": 50,
                            "roleId": ICLAPAudioBusConfigurationListModel.OutputCount
                        });
                        append({
                            "title": qsTr("Main In"),
                            "field": "clapabclm_input_count",
                            "columnWidth": 75,
                            "roleId": ICLAPAudioBusConfigurationListModel.InputCount
                        });
                        append({
                            "title": qsTr("Main Out"),
                            "field": "clapabclm_output_count",
                            "columnWidth": 75,
                            "roleId": ICLAPAudioBusConfigurationListModel.OutputCount
                        });
                    }
                }
                model: audioBusConfigurationListProxyModel
                listView.delegate: ItemDelegate {
                    id: itemDelegate
                    property string configId: clapabclm_id
                    width: Math.max(
                        audioChannelConfigList.listView.contentWidth,
                        audioChannelConfigList.listView.width
                    )
                    property var itemData: Array.isArray(audioChannelConfigList.listView.model)? modelData: model
                    Row {
                        Repeater {
                            model: audioChannelConfigList.headerListModel
                            Label {
                                width: columnWidth
                                leftPadding: (index == 0? itemDelegate.height: 2)
                                topPadding: 2
                                bottomPadding: 2
                                text: itemData[field]
                                clip: true
                                elide: Label.ElideRight
                            }
                        }
                    }
                }
            }
        }
        Item {
            width: impl.contentWidth
            height: defaultButton.height
            Button {
                id: defaultButton
                text: qsTr("&Default")
                onClicked: {
                    root.tryDefault();
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: okButton
                    text: Constants.okTextWithMnemonic
                    onClicked: {
                        root.accepted();
                    }
                }
                Button {
                    id: cancelButton
                    text: Constants.cancelTextWithMnemonic
                    onClicked: {
                        root.cancelled();
                    }
                }
            }
        }
    }
}