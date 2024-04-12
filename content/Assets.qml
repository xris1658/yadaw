import QtQml
import QtQuick
import QtQuick.Controls as QC
import QtQuick.Dialogs
import QtQuick.Layouts

import QtQml.Models

import YADAW.Models

Rectangle {
    id: root
    color: Colors.background
    border.color: Colors.controlBorder

    readonly property int preferredWidth: searchTextBox.height * 15
    property alias directoryListModel: directoryList.model
    property alias pluginListModel: pluginListProxyModel.sourceModel

    clip: true

    QtObject {
        id: impl
        function prepareSortOrder(model: SortFilterProxyListModel) {
            model.appendSortOrder(IPluginListModel.Format, Qt.AscendingOrder);
            model.appendSortOrder(IPluginListModel.Name, Qt.AscendingOrder);
        }
    }

    ListModel {
        id: assetHeaderListModel
        dynamicRoles: true
        Component.onCompleted: {
            append({
                "title": qsTr("Name"),
                "field": "ftm_name",
                "columnWidth": 197,
                "roleId": IFileTreeModel.Name
            });
        }
    }
    ListModel {
        id: pluginHeaderListModel
        dynamicRoles: true
        Component.onCompleted: {
            append({
                "title": qsTr("Name"),
                "field": "plm_name",
                "columnWidth": 197,
                "roleId": IPluginListModel.Name
            });
            append({
                "title": qsTr("Vendor"),
                "field": "plm_vendor",
                "columnWidth": 100,
                "roleId": IPluginListModel.Vendor
            });
        }
    }

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
                            assetTreeView.model = null;
                            directoryListModel.remove(pathId);
                        }
                        onClicked: {
                            rightLayout.currentIndex = 0;
                            directoryList.currentIndex = index;
                        }
                        onHighlightedChanged: {
                            if(highlighted) {
                                assetTreeView.model = adlm_file_tree;
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
            Component {
                id: pluginListItemDelegate
                ItemDelegate {
                    id: itemDelegate
                    width: Math.max(ListView.view.contentWidth, ListView.view.width)
                    property var itemData: Array.isArray(ListView.view.model)? modelData: model
                    Row {
                        Item {
                            id: formatIconPlaceholder
                            width: height
                            height: itemDelegate.height
                            StackLayout {
                                id: formatIcons
                                anchors.centerIn: parent
                                currentIndex: plm_format
                                layer.enabled: true
                                layer.smooth: true
                                layer.textureSize: Qt.size(width * 2, height * 2)
                                PluginIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                                VST3Icon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                                CLAPIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                                VestifalIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                            }
                        }
                        Item {
                            id: typeIconPlaceholder
                            width: height
                            height: itemDelegate.height
                            StackLayout {
                                id: typeIcons
                                anchors.centerIn: parent
                                currentIndex: plm_type
                                layer.enabled: true
                                layer.smooth: true
                                layer.textureSize: Qt.size(width * 2, height * 2)
                                PluginIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                                MIDIEffectIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                                PianoKeysIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                                AudioIcon {
                                    scale: 16 / originalHeight
                                    path.fillColor: Colors.secondaryContent
                                    path.strokeColor: "transparent"
                                }
                            }
                        }
                        Repeater {
                            model: pluginHeaderListModel
                            Label {
                                width: index == 0?
                                    columnWidth - formatIconPlaceholder.height - typeIconPlaceholder.height:
                                    columnWidth
                                leftPadding: index == 0? 0: 2
                                topPadding: 2
                                bottomPadding: 2
                                text: itemData[field]
                                elide: Label.ElideRight
                            }
                        }
                    }
                }
            }
            StackLayout {
                id: rightLayout
                anchors.fill: parent
                anchors.rightMargin: 1
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                TableLikeTreeView {
                    id: assetTreeView
                    headerListModel: assetHeaderListModel
                    treeView.boundsBehavior: Flickable.StopAtBounds
                    treeView.clip: true
                    treeView.delegate: ItemDelegate {
                        required property TreeView treeView
                        required property bool isTreeNode
                        required property bool expanded
                        required property int hasChildren
                        required property int depth
                        leftPadding: depth * height + indicator.width
                        width: assetTreeView.treeView.columnWidthProvider(column)
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
                            assetTreeView.treeView.toggleExpanded(row);
                        }
                    }
                }
                StackLayout {
                    id: categoriesLayout
                    currentIndex: categoriesLeftColumn.currentIndex
                    clip: true
                    TableLikeListView {
                        id: pluginListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            id: pluginListProxyModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                            }
                        }
                    }
                    TableLikeListView {
                        id: midiEffectListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                                setValueOfFilter(
                                    IPluginListModel.Type,
                                    IPluginListModel.MIDIEffect
                                );
                            }
                        }
                    }
                    TableLikeListView {
                        id: instrumentListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                                setValueOfFilter(
                                    IPluginListModel.Type,
                                    IPluginListModel.Instrument
                                );
                            }
                        }
                    }
                    TableLikeListView {
                        id: audioEffectListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                                setValueOfFilter(
                                    IPluginListModel.Type,
                                    IPluginListModel.AudioEffect
                                );
                            }
                        }
                    }
                    TableLikeListView {
                        id: vst3PluginListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                                setValueOfFilter(
                                    IPluginListModel.Format,
                                    IPluginListModel.VST3
                                );
                            }
                        }
                    }
                    TableLikeListView {
                        id: clapPluginListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                                setValueOfFilter(
                                    IPluginListModel.Format,
                                    IPluginListModel.CLAP
                                );
                            }
                        }
                    }
                    TableLikeListView {
                        id: vestifalPluginListView
                        listView.boundsBehavior: Flickable.StopAtBounds
                        listView.delegate: pluginListItemDelegate
                        headerListModel: pluginHeaderListModel
                        model: SortFilterProxyListModel {
                            sourceModel: root.pluginListModel
                            Component.onCompleted: {
                                impl.prepareSortOrder(this);
                                setValueOfFilter(
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
}
