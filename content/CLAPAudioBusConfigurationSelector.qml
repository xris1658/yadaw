import QtQuick

QC.Popup {
    id: root

    signal accepted()
    signal tryDefault()
    signal cancelled()

    property alias audioBusConfigurationList: audioBusConfigurationListProxyModel.sourceModel

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
        Item {
            width: impl.contentWidth
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
                    onCilcked: {
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