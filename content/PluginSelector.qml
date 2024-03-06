import QtQuick
import QtQuick.Controls as QC
import QtQuick.Layouts
import QtQuick.Shapes

import YADAW.Entities
import YADAW.Models

QC.Popup {
    id: root

    property alias pluginListModel: pluginList.model
    property alias categoryListModel: categoryList.model
    property bool replacing: false
    property bool enableReset: false

    signal accepted()
    signal cancelled()
    signal resetted()

    function currentPluginId() {
        return pluginList.currentItem.pluginId;
    }

    function currentPluginName() {
        return pluginList.currentItem.pluginName;
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
            onTextChanged: {
                pluginListModel.filterString = text;
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
                                    layer.samples: 4
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
                                    pluginListModel.clearValueOfFilter(IPluginListModel.Format);
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
                            model: ListModel {
                                ListElement {
                                    name: "Vestifal"
                                    iconSource: "VestifalIcon.qml"
                                    format: PluginFormatSupport.Vestifal
                                    filterableFormat: IPluginListModel.Vestifal
                                }
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
                                    layer.samples: 4
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
                                    pluginListModel.setValueOfFilter(IPluginListModel.Format, filterableFormat);
                                    // pluginListModel.setfilterRole(IPluginListModel)
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
                                    layer.samples: 4
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
                Item {
                    ListView {
                        id: pluginList
                        anchors.fill: parent
                        anchors.rightMargin: vbar.visible? vbar.width: 0
                        anchors.bottomMargin: hbar.visible? hbar.height: 0
                        ScrollBar.vertical: vbar
                        ScrollBar.horizontal: hbar
                        clip: true
                        flickableDirection: Flickable.AutoFlickDirection
                        boundsBehavior: Flickable.StopAtBounds
                        property ListModel headerListModel: ListModel {
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
                        header: ListView {
                            z: 2
                            id: headerListView
                            width: pluginList.width
                            property int textPadding: 2
                            property int minimumColumnWidth: 10
                            interactive: false
                            model: pluginList.headerListModel
                            visible: pluginList.count  !== 0
                            function getWidth(index: int) {
                                return model.get(index).columnWidth;
                            }
                            orientation: ListView.Horizontal
                            Rectangle {
                                z: -1
                                id: headerBackground
                                width: Math.max(parent.contentWidth, parent.width)
                                height: parent.height
                                color: Colors.controlBackground
                                Rectangle {
                                    anchors.bottom: parent.bottom
                                    width: parent.width
                                    height: 1
                                    color: Colors.border
                                }
                            }
                            delegate: MouseArea {
                                id: headerItemDelegate
                                height: headerLabel.height
                                acceptedButtons: Qt.LeftButton
                                Label {
                                    id: headerLabel
                                    leftPadding: headerListView.textPadding
                                    rightPadding: headerMouseArea.width
                                    topPadding: headerListView.textPadding
                                    bottomPadding: headerListView.textPadding
                                    text: title
                                    clip: true
                                }
                                Label {
                                    id: sortOrderLabel
                                    anchors.right: parent.right
                                    anchors.rightMargin: headerListView.textPadding
                                    anchors.verticalCenter: parent.verticalCenter
                                    font.pointSize: Qt.application.font.pointSize * 0.75
                                    text: "\u25bc"
                                    clip: true
                                    color: Colors.secondaryContent
                                }
                                Rectangle {
                                    anchors.right: parent.right
                                    width: 1
                                    height: parent.height
                                    color: Colors.border
                                }
                                MouseArea {
                                    property double originalMouseX
                                    id: headerMouseArea
                                    anchors.right: parent.right
                                    width: 5
                                    height: parent.height
                                    cursorShape: Qt.SizeHorCursor
                                    preventStealing: true
                                    onPressed: (mouse) => {
                                        originalMouseX = mouseX;
                                    }
                                    onMouseXChanged: (mouse) => {
                                        const delta = mouseX - originalMouseX;
                                        if(headerItemDelegate.width + delta < headerListView.minimumColumnWidth) {
                                            headerItemDelegate.width = headerListView.width;
                                        }
                                        else {
                                            headerItemDelegate.width += delta;
                                            originalMouseX = mouseX;
                                        }
                                    }
                                }
                                onWidthChanged: {
                                    headerListView.model.setProperty(index, "columnWidth", width);
                                }
                                onClicked: (mouse) => {
                                    if(mouse.modifiers & Qt.ControlModifier) {
                                        //
                                    }
                                    // TODO:
                                    //  Behavior w/o Ctrl/Cmd modifier: Remove other sorts, toggle column sort as not sorted -> ascending -> descending -> not sorted
                                    //  Behavior w/  Ctrl/Cmd modifier: add/remove sort order
                                }
                                Component.onCompleted: {
                                    // width = headerLabel.width * 2;
                                    width = headerListView.model.get(index).columnWidth;
                                    if(index === 0) {
                                        headerListView.height = height;
                                    }
                                }
                            }
                            onHeightChanged: {
                                vbar.anchors.topMargin = height;
                                headerBackground.height = height;
                                // `ListView` always flicks item delegates to Y of 0, making the
                                // first item covered by the header. This happens even if
                                // `headerPositioning` is set to `ListView.OverlayHeader`,
                                // The following line is a workaround to correct the initial
                                // position of those delegates.
                                pluginList.contentY = -1 * height;
                            }
                            Component.onCompleted: {
                                pluginList.contentWidth = contentWidth;
                            }
                            onContentWidthChanged: {
                                pluginList.contentWidth = contentWidth;
                            }
                        }
                        headerPositioning: ListView.OverlayHeader
                        delegate: ItemDelegate {
                            id: itemDelegate
                            property int pluginId: plm_id
                            property string pluginName: plm_name
                            width: Math.max(pluginList.contentWidth, pluginList.width)
                            property var itemData: Array.isArray(pluginList.model)? modelData: model
                            Item {
                                width: height
                                height: itemDelegate.height
                                StackLayout {
                                    id: formatIcons
                                    anchors.centerIn: parent
                                    currentIndex: plm_format
                                    layer.enabled: true
                                    layer.smooth: true
                                    layer.samples: 4
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
                                pluginList.currentIndex = index;
                            }
                            onDoubleClicked: {
                                pluginList.currentIndex = index;
                                root.accepted();
                            }
                        }
                        highlight: Rectangle {
                            color: Colors.highlightControlBackground
                        }
                        highlightFollowsCurrentItem: true
                        highlightMoveDuration: 0
                        Grid {
                            columns: 1
                            anchors.centerIn: parent
                            visible: pluginList.count === 0
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
                        onModelChanged: {
                            currentIndex = -1;
                        }
                    }
                    ScrollBar {
                        id: vbar
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.topMargin: pluginList.header.height
                        anchors.bottom: pluginList.bottom
                        visible: pluginList.height < pluginList.contentHeight
                        orientation: Qt.Vertical
                    }
                    ScrollBar {
                        id: hbar
                        anchors.right: pluginList.right
                        anchors.bottom: parent.bottom
                        width: pluginList.width
                        visible: pluginList.width < pluginList.contentWidth
                        orientation: Qt.Horizontal
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
                    enabled: pluginList.currentIndex !== -1 && visible
                    visible: root.replacing
                    text: qsTr("&Replace")
                    onClicked: {
                        root.accepted();
                    }
                }
                Button {
                    id: insertButton
                    enabled: pluginList.currentIndex !== -1 && visible
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
            pluginListModel.setFilter(pluginList.headerListModel.get(i).roleId, true);
        }
        pluginListModel.appendSortOrder(IPluginListModel.Name, Qt.AscendingOrder);
    }
}
