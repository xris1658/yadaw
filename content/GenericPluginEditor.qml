import QtQml
import QtQuick
import QtQuick.Layouts

Window {
    id: root

    property bool destroyingPlugin: false
    property alias pluginName: titleLabel.text
    property alias parameterListModel: parameterList.model

    flags: Qt.Dialog
    modality: Qt.NonModal
    width: 400
    height: 400
    color: Colors.background
    Item {
        id: header
        width: root.width
        height: titleLabel.contentHeight + 6
        Label {
            id: titleLabel
            font.pointSize: Qt.application.font.pointSize * 1.5
            anchors.fill: parent
            anchors.margins: 3
            text: "Plugin Name Here"
        }
        Rectangle {
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
            color: Colors.border
        }
    }
    Item {
        width: parent.width
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        ListView {
            id: parameterList
            anchors.fill: parent
            anchors.rightMargin: vbar.visible? vbar.width: 0
            anchors.bottomMargin: hbar.visible? hbar.height: 0
            orientation: Qt.Vertical
            clip: true
            ScrollBar.vertical: vbar
            ScrollBar.horizontal: hbar
            flickableDirection: Flickable.AutoFlickDirection
            boundsBehavior: Flickable.StopAtBounds
            delegate: Item {
                width: parent.width
                 height: label.height + 5 * 2
                visible: pplm_is_visible
                Label {
                    id: label
                    text: pplm_name
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                }
                Slider {
                    anchors.right: parent.right
                    visible: !(pplm_show_as_list || pplm_show_as_switch)
                    anchors.verticalCenter: parent.verticalCenter
                    from: pplm_min_value
                    to: pplm_max_value
                    value: pplm_value
                    stepSize: pplm_step_size
                    snapMode: Slider.SnapAlways
                }
                ComboBox {
                    id: comboBox
                    enabled: pplm_show_as_list
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: enabled? implicitWidth: 0
                    model: pplm_value_and_text_list
                    textRole: "pvatlm_text"
                    valueRole: "pvatlm_value"
                    currentIndex: indexOfValue(pplm_value)
                }
                Switch {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    visible: pplm_show_as_switch
                    checked: pplm_value
                }
            }
            reuseItems: true
        }
        ScrollBar {
            id: vbar
            anchors.top: parent.top
            anchors.right: parent.right
            visible: parameterList.height < parameterList.contentHeight
            height: parameterList.height
            orientation: Qt.Vertical
        }
        ScrollBar {
            id: hbar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            visible: parameterList.width < parameterList.contentWidth
            width: parameterList.width
            orientation: Qt.Horizontal
        }
    }
    onClosing: (close) => {
        if(!root.destroyingPlugin) {
            close.accepted = false;
            root.hide();
        }
    }
}
