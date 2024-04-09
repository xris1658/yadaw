import QtQml
import QtQuick
import QtQuick.Dialogs
import QtQuick.Layouts

import QtQuick.Controls as QC

import YADAW.Models

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.controlBorder

    readonly property int preferredWidth: searchTextBox.height * 15

    property alias directoryListModel: directoryList.model

    property alias pluginListModel: pluginListView.model
    property alias midiEffectListModel: midiEffectListView.model
    property alias instrumentListModel: instrumentListView.model
    property alias audioEffectListModel: audioEffectListView.model
    property alias vestifalPluginListModel: vestifalPluginListView.model
    property alias vst3PluginListModel: vst3PluginListView.model
    property alias clapPluginListModel: clapPluginListView.model

    clip: true

    Menu {
        id: assetDirectoryOptions
        title: qsTr("Asset directory options")
        Action {
            text: qsTr("&Locate in Explorer")
            onTriggered: {
                EventSender.locatePathInExplorer(assetDirectoryOptions.parent.pathPath);
            }
        }
        Action {
            text: qsTr("Rena&me")
            onTriggered: {
                assetDirectoryOptions.parent.rename();
            }
        }
        Action {
            text: qsTr("Remove from &Directories")
            onTriggered: {
                assetDirectoryOptions.parent.remove();
            }
        }
    }

    TextField {
        id: assetDirectoryRenameTextField
        visible: false
        padding: 2
        Keys.onEscapePressed: (event) => {
            visible = false;
        }
        onAccepted: {
            if(text.length === 0) {
                text = placeholderText;
            }
            parent.renameAccepted(text);
            visible = false;
        }
    }

    TextField {
        id: searchTextBox
        anchors.top: parent.top
        width: parent.width
        height: implicitHeight + 1 // align bottom width the arrangement view
        placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
        Button {
            id: clearButton
            visible: parent.length !== 0
            anchors.right: parent.right
            anchors.rightMargin: 1
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: parent.height - anchors.rightMargin * 2
            text: "\u232b"
            padding: 1
            background: Rectangle {
                anchors.fill: parent
                color: (!clearButton.enabled)? "transparent":
                    clearButton.down? Colors.pressedControlBackground:
                    clearButton.hovered? Colors.mouseOverControlBackground:
                    "transparent"
            }
            onClicked: {
                parent.clear();
            }
        }
    }
    SplitView {
        anchors.top: searchTextBox.bottom
        anchors.topMargin: -1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        orientation: Qt.Horizontal
        handle: Item {
            implicitWidth: 5
            implicitHeight: 5
            Rectangle {
                width: 1
                height: parent.height
                anchors.right: parent.right
                color: Colors.controlBorder
            }
        }
        Rectangle {
            SplitView.preferredWidth: 100
            color: "transparent"
            clip: true
            Column {
                topPadding: 5
                bottomPadding: 5
                width: parent.width
                Label {
                    text: qsTr("Locations")
                    color: Colors.secondaryContent
                    font.pointSize: Qt.application.font.pointSize * 0.9
                    width: parent.width
                    height: contentHeight + topPadding + bottomPadding
                    leftPadding: 5
                    rightPadding: leftPadding
                    topPadding: 3
                    bottomPadding: 0
                    elide: Text.ElideRight
                }
                ListView {
                    id: directoryList
                    width: parent.width
                    height: contentHeight
                    delegate: ItemDelegate {
                        id: control
                        property string pathId: adlm_id
                        property string pathPath: adlm_path
                        property string pathName: adlm_name
                        property string defaultName: adlm_default_name
                        width: parent? parent.width - 3 * 2: 0
                        x: root.border.width + 3
                        text: pathName
                        leftPadding: height
                        rightPadding: 2
                        topPadding: 2
                        bottomPadding: 2
                        highlighted: rightLayout.currentIndex === 0 && directoryList.currentIndex === index
                        background: Rectangle {
                            width: control.width
                            height: control.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!control.enabled)? Colors.background:
                                control.highlighted? Colors.highlightControlBackground:
                                control.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.RightButton
                            onClicked: {
                                assetDirectoryOptions.parent = control;
                                assetDirectoryOptions.x = mouseX;
                                assetDirectoryOptions.y = mouseY;
                                assetDirectoryOptions.open();
                                assetDirectoryOptions.forceActiveFocus();
                            }
                        }
                        Item {
                            anchors.left: parent.left
                            width: height
                            height: parent.height
                            Item {
                                anchors.centerIn: parent
                                width: 16
                                height: width
                                FolderIcon {
                                    anchors.centerIn: parent
                                    scale: parent.height / originalHeight
                                    path.strokeColor: "transparent"
                                    path.fillColor: Colors.secondaryContent
                                }
                            }
                        }
                        signal rename()
                        onRename: {
                            assetDirectoryRenameTextField.parent = this;
                            assetDirectoryRenameTextField.visible = true;
                            assetDirectoryRenameTextField.x = control.height - assetDirectoryRenameTextField.leftPadding;
                            assetDirectoryRenameTextField.width = control.width - control.height + assetDirectoryRenameTextField.leftPadding;
                            assetDirectoryRenameTextField.height = control.height;
                            assetDirectoryRenameTextField.text = control.pathName;
                            assetDirectoryRenameTextField.placeholderText = control.defaultName;
                            // assetDirectoryRenameTextField.select(assetDirectoryRenameTextField.length, 0);
                            assetDirectoryRenameTextField.selectAll();
                            assetDirectoryRenameTextField.forceActiveFocus();
                            assetDirectoryRenameTextField.visible = true;
                        }
                        signal renameAccepted(name: string)
                        onRenameAccepted: (name) => {
                            directoryListModel.rename(pathId, name);
                        }
                        signal remove()
                        onRemove: {
                            assetListView.model = null;
                            directoryListModel.remove(pathId);
                        }
                        onClicked: {
                            rightLayout.currentIndex = 0;
                            directoryList.currentIndex = index;
                        }
                        onHighlightedChanged: {
                            if(highlighted) {
                                assetListView.model = adlm_file_tree;
                            }
                        }
                    }
                    footer: ItemDelegate {
                        id: addLocation
                        width: parent.width - 3 * 2
                        x: root.border.width + 3
                        text: qsTr("Add...")
                        rightPadding: 2
                        background: Rectangle {
                            width: addLocation.width
                            height: addLocation.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!addLocation.enabled)? Colors.background:
                                addLocation.highlighted? Colors.highlightControlBackground:
                                addLocation.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                        FolderDialog {
                            id: folderDialog
                            onAccepted: {
                                directoryListModel.append(folderDialog.selectedFolder);
                            }
                        }
                        onClicked: {
                            folderDialog.open();
                        }
                    }
                }
                Label {
                    text: qsTr("Categories")
                    color: Colors.secondaryContent
                    font.pointSize: Qt.application.font.pointSize * 0.9
                    width: parent.width
                    height: contentHeight + topPadding + bottomPadding
                    leftPadding: 5
                    rightPadding: leftPadding
                    topPadding: 3
                    bottomPadding: 0
                    elide: Text.ElideRight
                }
                ListView {
                    id: categoriesLeftColumn
                    model: ListModel {
                        ListElement {
                            name: qsTr("Plugins")
                            iconSource: "PluginIcon.qml"
                        }
                        ListElement {
                            name: qsTr("MIDI Effects")
                            iconSource: "MIDIEffectIcon.qml"
                        }
                        ListElement {
                            name: qsTr("Instruments")
                            iconSource: "PianoKeysIcon.qml"
                        }
                        ListElement {
                            name: qsTr("Audio Effects")
                            iconSource: "AudioIcon.qml"
                        }
                        ListElement {
                            name: "VST3"
                            iconSource: "VST3Icon.qml"
                        }
                        ListElement {
                            name: "CLAP"
                            iconSource: "CLAPIcon.qml"
                        }
                        ListElement {
                            name: "Vestifal"
                            iconSource: "VestifalIcon.qml"
                        }
                    }

                    // model: ["Plugins", "MIDI Effects", "Instruments", "Audio Effects", "Audio Files", "MIDI Files"]
                    width: parent.width
                    height: contentHeight
                    delegate: ItemDelegate {
                        id: pluginItemDelegate
                        width: parent.width - 3 * 2
                        height: implicitHeight
                        x: root.border.width + 3
                        text: name
                        leftPadding: height
                        rightPadding: 2
                        topPadding: 2
                        bottomPadding: 2
                        highlighted: rightLayout.currentIndex === 1 && categoriesLeftColumn.currentIndex === index
                        background: Rectangle {
                            width: pluginItemDelegate.width
                            height: pluginItemDelegate.height
                            anchors.topMargin: root.topInset
                            anchors.bottomMargin: root.bottomInset
                            anchors.leftMargin: root.leftInset
                            anchors.rightMargin: root.rightInset
                            color: (!pluginItemDelegate.enabled)? Colors.background:
                                pluginItemDelegate.highlighted? Colors.highlightControlBackground:
                                pluginItemDelegate.hovered? Colors.mouseOverControlBackground: Colors.background
                        }
                        // Only the icon is smoothed; the text is not affected.
                        Item {
                            id: enableLayerForIcons
                            width: height
                            height: parent.height
                            layer.enabled: true
                            layer.smooth: true
                            layer.textureSize: Qt.size(width * 2, height * 2)
                            Loader {
                                id: loader
                                onLoaded: {
                                    item.parent = enableLayerForIcons;
                                    item.scale = 16 / item.originalHeight;
                                    item.anchors.centerIn = enableLayerForIcons;
                                    item.path.strokeColor = "transparent";
                                    item.path.fillColor = Colors.secondaryContent;
                                }
                            }
                        }
                        Component.onCompleted: {
                            loader.source = iconSource;
                        }
                        onClicked: {
                            rightLayout.currentIndex = 1;
                            categoriesLeftColumn.currentIndex = index;
                        }
                    }
                }
            }
        }
        Rectangle {
        color: "transparent"
            StackLayout {
                id: rightLayout
                anchors.fill: parent
                anchors.rightMargin: 1
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                TreeView {
                    id: assetListView
                    clip: true
                    columnWidthProvider: (column) => {
                        return assetListView.width;
                    }
                    delegate: ItemDelegate {
                        required property TreeView treeView
                        required property bool isTreeNode
                        required property bool expanded
                        required property int hasChildren
                        required property int depth
                        leftPadding: depth * height + indicator.width
                        Label {
                            id: indicator
                            x: depth * parent.height
                            width: height
                            height: parent.height
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignHCenter
                            visible: parent.isTreeNode && parent.hasChildren
                            text: parent.expanded? "\u25bc": "\u25b6"
                        }
                        text: ftm_name
                        onClicked: {
                            assetListView.toggleExpanded(row);
                        }
                    }
                }
                StackLayout {
                    id: categoriesLayout
                    currentIndex: categoriesLeftColumn.currentIndex
                    clip: true
                    PluginListView {
                        id: pluginListView
                    }
                    PluginListView {
                        id: midiEffectListView
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                        }
                        Component.onCompleted: {
                            model.setValueOfFilter(
                                IPluginListModel.Type,
                                IPluginListModel.MIDIEffect
                            );
                        }
                    }
                    PluginListView {
                        id: instrumentListView
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                        }
                        Component.onCompleted: {
                            model.setValueOfFilter(
                                IPluginListModel.Type,
                                IPluginListModel.Instrument
                            );
                        }
                    }
                    PluginListView {
                        id: audioEffectListView
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                        }
                        Component.onCompleted: {
                            model.setValueOfFilter(
                                IPluginListModel.Type,
                                IPluginListModel.AudioEffect
                            );
                        }
                    }
                    PluginListView {
                        id: vst3PluginListView
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                        }
                        Component.onCompleted: {
                            model.setValueOfFilter(
                                IPluginListModel.Format,
                                IPluginListModel.VST3
                            );
                        }
                    }
                    PluginListView {
                        id: clapPluginListView
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                        }
                        Component.onCompleted: {
                            model.setValueOfFilter(
                                IPluginListModel.Format,
                                IPluginListModel.CLAP
                            );
                        }
                    }
                    PluginListView {
                        id: vestifalPluginListView
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                        }
                        Component.onCompleted: {
                            model.setValueOfFilter(
                                IPluginListModel.Format,
                                IPluginListModel.Vestifal
                            );
                        }
                    }
                }
            }
        }
    }
}
