import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Shapes

import YADAW.Entities
import YADAW.Models

QC.Popup {
    id: root

    property alias pluginListProxyModel: pluginListProxyModel
    property alias categoryListModel: categoryList.model
    property bool replacing: false
    property bool enableReset: false

    signal accepted()
    signal cancelled()
    signal resetted()

    function currentPluginId(): int {
        return pluginList.listView.currentItem.pluginId;
    }

    function currentPluginName(): string {
        return pluginList.listView.currentItem.pluginName;
    }

    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    padding: 10

    QtObject {
        id: impl
        property int contentWidth: 600
        readonly property PluginFormatSupport pluginFormatSupport: PluginFormatSupport {}
    }

    SortFilterProxyListModel {
        id: pluginListProxyModel
        filterString: searchTextField.text
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
        TextField {
            id: searchTextField
            placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
            width: impl.contentWidth
            Keys.onDownPressed: (event) => {
                if(pluginList.listView.count !== 0) {
                    pluginList.listView.forceActiveFocus();
                    if(pluginList.listView.currentIndex == -1) {
                        pluginList.listView.currentIndex = 0;
                    }
                }
            }
        }
        Rectangle {
            width: impl.contentWidth
            height: 300
            color: "transparent"
            border.color: Colors.border
            SplitView {
                anchors.fill: parent
                anchors.margins: parent.border.width
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
                Item {
                    id: leftLists
                    property int currentIndex: 0
                    SplitView.preferredWidth: 100
                    Column {
                        topPadding: 5
                        bottomPadding: 5
                        x: 5
                        width: parent.width
                        ListView {
                            id: allList
                            model: [qsTr("All")]
                            width: parent.width
                            height: contentHeight
                            activeFocusOnTab: leftLists.currentIndex === 0
                            delegate: ItemDelegate {
                                id: allItemDelegate
                                width: parent.width - 3 * 2
                                height: implicitHeight
                                text: modelData
                                leftPadding: height
                                rightPadding: 2
                                topPadding: 2
                                bottomPadding: 2
                                highlighted: leftLists.currentIndex == 0
                                background: Rectangle {
                                    width: allItemDelegate.width
                                    height: allItemDelegate.height
                                    anchors.topMargin: root.topInset
                                    anchors.bottomMargin: root.bottomInset
                                    anchors.leftMargin: root.leftInset
                                    anchors.rightMargin: root.rightInset
                                    color: (!allItemDelegate.enabled)? Colors.background:
                                        allItemDelegate.highlighted? Colors.highlightControlBackground:
                                        allItemDelegate.hovered? Colors.mouseOverControlBackground: Colors.background
                                }
                                // Only the icon is smoothed; the text is not affected.
                                Item {
                                    width: height
                                    height: parent.height
                                    layer.enabled: true
                                    layer.smooth: true
                                    layer.textureSize: Qt.size(width * 2, height * 2)
                                    PluginIcon {
                                        scale: 16 / originalHeight
                                        anchors.centerIn: parent
                                        path.strokeColor: "transparent"
                                        path.fillColor: Colors.secondaryContent;
                                        path.fillRule: ShapePath.OddEvenFill
                                    }
                                }
                                onClicked: {
                                    leftLists.currentIndex = 0;
                                    pluginListProxyModel.clearValueOfFilter(IPluginListModel.Format);
                                }
                                Keys.onDownPressed: {
                                    leftLists.currentIndex = 1;
                                    formatList.currentIndex = 0;
                                    formatList.forceActiveFocus();
                                }
                            }
                        }
                        Label {
                            text: qsTr("Format")
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
                            id: formatList
                            currentIndex: -1
                            activeFocusOnTab: leftLists.currentIndex === 1
                            model: ListModel {
                                ListElement {
                                    name: "VST3"
                                    iconSource: "VST3Icon.qml"
                                    format: PluginFormatSupport.VST3
                                    filterableFormat: IPluginListModel.VST3
                                }
                                ListElement {
                                    name: "CLAP"
                                    iconSource: "CLAPIcon.qml"
                                    format: PluginFormatSupport.CLAP
                                    filterableFormat: IPluginListModel.CLAP
                                }
                                ListElement {
                                    name: "Vestifal"
                                    iconSource: "VestifalIcon.qml"
                                    format: PluginFormatSupport.Vestifal
                                    filterableFormat: IPluginListModel.Vestifal
                                }
                            }
                            width: parent.width
                            height: contentHeight
                            delegate: ItemDelegate {
                                id: pluginItemDelegate
                                property int format: filterableFormat
                                width: parent.width - 3 * 2
                                height: implicitHeight
                                text: name
                                leftPadding: height
                                rightPadding: 2
                                topPadding: 2
                                bottomPadding: 2
                                highlighted: leftLists.currentIndex == 1 && formatList.currentIndex === index
                                visible: impl.pluginFormatSupport.isPluginFormatSupported(format)
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
                                    leftLists.currentIndex = 1;
                                    formatList.currentIndex = index;
                                    // pluginListProxyModel.setValueOfFilter(IPluginListModel.Format, filterableFormat);
                                    // pluginListProxyModel.setfilterRole(IPluginListModel)
                                }
                            }
                            onCurrentIndexChanged: {
                                if(currentIndex !== -1) {
                                    pluginListProxyModel.setValueOfFilter(
                                        IPluginListModel.Format,
                                        itemAtIndex(currentIndex).format
                                    );
                                }
                            }
                            Keys.onUpPressed: (event) => {
                                if(formatList.currentIndex == 0) {
                                    formatList.currentIndex = -1;
                                    allList.currentIndex = 0;
                                    leftLists.currentIndex = 0;
                                    pluginListProxyModel.clearValueOfFilter(IPluginListModel.Format);
                                    allList.forceActiveFocus();
                                }
                                else {
                                    event.accepted = false;
                                }
                            }
                        }
                        Label {
                            text: qsTr("Categories")
                            visible: categoryList.count !== 0
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
                            id: categoryList
                            width: parent.width
                            height: contentHeight
                            delegate: ItemDelegate {
                                id: categoryItemDelegate
                                width: parent.width - 3 * 2
                                height: implicitHeight
                                text: modelData
                                leftPadding: height
                                rightPadding: 2
                                topPadding: 2
                                bottomPadding: 2
                                highlighted: leftLists.currentIndex == 2 && categoryList.currentIndex === index
                                background: Rectangle {
                                    width: categoryItemDelegate.width
                                    height: categoryItemDelegate.height
                                    anchors.topMargin: root.topInset
                                    anchors.bottomMargin: root.bottomInset
                                    anchors.leftMargin: root.leftInset
                                    anchors.rightMargin: root.rightInset
                                    color: (!categoryItemDelegate.enabled)? Colors.background:
                                        categoryItemDelegate.highlighted? Colors.highlightControlBackground:
                                        categoryItemDelegate.hovered? Colors.mouseOverControlBackground: Colors.background
                                }
                                // Only the icon is smoothed; the text is not affected.
                                Item {
                                    width: height
                                    height: parent.height
                                    layer.enabled: true
                                    layer.smooth: true
                                    layer.textureSize: Qt.size(width * 2, height * 2)
                                    TagIcon {
                                        scale: 16 / originalHeight
                                        anchors.centerIn: parent
                                        path.strokeColor: "transparent"
                                        path.fillColor: Colors.secondaryContent;
                                        path.fillRule: ShapePath.OddEvenFill
                                    }
                                }
                                onClicked: {
                                    leftLists.currentIndex = 2;
                                    categoryList.currentIndex = index;
                                }
                            }
                        }
                    }
                }
                TableLikeListView {
                    id: pluginList
                    listView.activeFocusOnTab: true
                    listView.flickableDirection: Flickable.AutoFlickDirection
                    listView.boundsBehavior: Flickable.StopAtBounds
                    listView.reuseItems: true
                    headerListModel: ListModel {
                        dynamicRoles: true
                        Component.onCompleted: {
                            append({
                                "title": qsTr("Name"),
                                "field": "plm_name",
                                "columnWidth": 200,
                                "roleId": IPluginListModel.Name
                            });
                            append({
                                "title": qsTr("Vendor"),
                                "field": "plm_vendor",
                                "columnWidth": 181,
                                "roleId": IPluginListModel.Vendor
                            });
                            append({
                                "title": qsTr("Version"),
                                "field": "plm_version",
                                "columnWidth": 100,
                                "roleId": IPluginListModel.Version
                            });
                        }
                    }
                    model: pluginListProxyModel
                    listView.delegate: ItemDelegate {
                        id: itemDelegate
                        property int pluginId: plm_id
                        property string pluginName: plm_name
                        width: Math.max(pluginList.listView.contentWidth, pluginList.listView.width)
                        property var itemData: Array.isArray(pluginList.listView.model)? modelData: model
                        Item {
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
                        Row {
                            Repeater {
                                model: pluginList.headerListModel
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
                        onClicked: {
                            pluginList.listView.currentIndex = index;
                        }
                        onDoubleClicked: {
                            pluginList.listView.currentIndex = index;
                            root.accepted();
                        }
                    }
                    Grid {
                        columns: 1
                        parent: pluginList.listView
                        anchors.centerIn: parent
                        visible: pluginList.listView.count === 0
                        horizontalItemAlignment: Grid.AlignHCenter
                        Item {
                            width: 128
                            height: width
                            PluginIcon {
                                anchors.centerIn: parent
                                scale: parent.width / originalWidth
                                path.fillColor: Colors.secondaryContent
                                path.strokeColor: "transparent"
                            }
                        }
                        Label {
                            font.pointSize: Qt.application.font.pointSize * 1.5
                            text: qsTr("No plugins available")
                            color: Colors.secondaryContent
                        }
                    }
                    listView.onModelChanged: {
                        listView.currentIndex = -1;
                    }
                    listView.Keys.onUpPressed: (event) => {
                        if(listView.currentIndex == 0) {
                            searchTextField.forceActiveFocus();
                            listView.currentIndex = -1;
                        }
                        else {
                            event.accepted = false;
                        }
                    }
                    listView.onCurrentIndexChanged: {
                        if(listView.highlightItem !== null) {
                            // Make sure the item at current index is not
                            // covered by header.
                            // (Seriously, why doesn't `ListView` do this by
                            // default when `headerPositioning` is
                            // `ListView.OverlayHeader`?)
                            let currentY = listView.highlightItem.mapToItem(pluginList, 0, 0).y;
                            // We cannot get `header.height` directly,
                            // so we use properties of `vbar`
                            let headerHeight = pluginList.vbar.anchors.topMargin;
                            if(currentY < headerHeight) {
                                pluginList.listView.contentY -= (headerHeight - currentY);
                            }
                        }
                    }
                }
            }
        }
        Item {
            width: impl.contentWidth
            height: cancelButton.height
            Button {
                id: resetButton
                visible: root.enableReset
                text: qsTr("R&eset")
                onClicked: {
                    root.resetted();
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: replaceButton
                    enabled: pluginList.listView.currentIndex !== -1 && visible
                    visible: root.replacing
                    text: qsTr("&Replace")
                    onClicked: {
                        root.accepted();
                    }
                }
                Button {
                    id: insertButton
                    enabled: pluginList.listView.currentIndex !== -1 && visible
                    visible: !root.replacing
                    text: qsTr("&Insert")
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
    onOpened: {
        searchTextField.forceActiveFocus();
        searchTextField.selectAll();
        for(let i = 0; i < pluginList.headerListModel.count; ++i) {
            pluginListProxyModel.setFilter(pluginList.headerListModel.get(i).roleId, true);
        }
        // VST3 > CLAP > Vestifal
        pluginListProxyModel.appendSortOrder(IPluginListModel.Format, Qt.AscendingOrder);
        pluginListProxyModel.appendSortOrder(IPluginListModel.Name, Qt.AscendingOrder);
    }
    Shortcut {
        sequence: "Ctrl+F"
        onActivated: {
            searchTextField.forceActiveFocus();
        }
    }
}
