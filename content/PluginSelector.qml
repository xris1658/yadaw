import QtQuick
import QtQuick.Controls as QC
import QtQuick.Shapes

QC.Popup {
    id: root

    property alias pluginListModel: pluginList.model
    property alias categoryListModel: categoryList.model

    QtObject {
        id: impl
        property int contentWidth: 600
    }

    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0
    padding: 10

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
            placeholderText: "<i>" + qsTr("Search (Ctrl+F)") + "</i>"
            width: impl.contentWidth
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
                                }
                                ListElement {
                                    name: "VST3"
                                    iconSource: "VST3Icon.qml"
                                }
                                ListElement {
                                    name: "CLAP"
                                    iconSource: "CLAPIcon.qml"
                                }
                            }
                            width: parent.width
                            height: contentHeight
                            delegate: ItemDelegate {
                                id: pluginItemDelegate
                                width: parent.width - 3 * 2
                                height: implicitHeight
                                text: name
                                leftPadding: height
                                rightPadding: 2
                                topPadding: 2
                                bottomPadding: 2
                                highlighted: leftLists.currentIndex == 1 && formatList.currentIndex === index
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
                    id: rightContainer
                    Column {
                        Rectangle {
                            id: header
                            color: Colors.controlBackground
                            width: hbar.width
                            height: headerListView.height
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 1
                                color: Colors.border
                            }
                            ListView {
                                id: headerListView
                                width: parent.width
                                property int textPadding: 2
                                property int minimumColumnWidth: 10
                                // interactive: false
                                model: ListModel {
                                    dynamicRoles: true
                                    Component.onCompleted: {
                                        append({
                                            "title": qsTr("Name"),
                                            "field": "plm_name",
                                            "columnWidth": 100
                                        });
                                        append({
                                            "title": qsTr("Vendor"),
                                            "field": "plm_vendor",
                                            "columnWidth": 100
                                        });
                                        append({
                                            "title": qsTr("Version"),
                                            "field": "plm_version",
                                            "columnWidth": 100
                                        });
                                    }
                                }
                                function getWidth(index: int) {
                                    return model.get(index).columnWidth;
                                }
                                orientation: ListView.Horizontal
                                delegate: Item {
                                    id: headerItemDelegate
                                    height: headerLabel.height
                                    Label {
                                        id: headerLabel
                                        leftPadding: headerListView.textPadding
                                        rightPadding: headerMouseArea.width
                                        topPadding: headerListView.textPadding
                                        bottomPadding: headerListView.textPadding
                                        text: title
                                        clip: true
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
                                        onMouseXChanged:
                                            (mouse) => {
                                                if(true) {
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
                                    }
                                    onWidthChanged: {
                                        headerListView.model.setProperty(index, "columnWidth", width);
                                    }
                                    Component.onCompleted: {
                                        width = headerLabel.width * 2;
                                        if(index === 0) {
                                            headerListView.height = height;
                                        }
                                    }
                                }
                            }
                        }
                        Item {
                            width: rightContainer.width
                            height: rightContainer.height - header.height
                            ListView {
                                id: pluginList
                                anchors.fill: parent
                                anchors.rightMargin: vbar.width
                                anchors.bottomMargin: hbar.height
                                ScrollBar.vertical: vbar
                                ScrollBar.horizontal: hbar
                                clip: true
                                delegate: ItemDelegate {
                                    id: itemDelegate
                                    width: headerListView.width
                                    property var itemData: Array.isArray(pluginList.model)? modelData: model
                                    Row {
                                        Repeater {
                                            model: headerListView.model
                                            Label {
                                                width: headerListView.getWidth(index)
                                                leftPadding: headerListView.textPadding
                                                topPadding: headerListView.textPadding
                                                bottomPadding: headerListView.textPadding
                                                text: itemData[field]
                                                clip: true
                                                elide: Label.ElideRight
                                            }
                                        }
                                    }
                                }
                            }
                            ScrollBar {
                                id: vbar
                                anchors.right: parent.right
                                width: pluginList.contentHeight > pluginList.height? implicitWidth: 0
                                height: pluginList.height
                                orientation: Qt.Vertical
                            }
                            ScrollBar {
                                id: hbar
                                anchors.bottom: parent.bottom
                                width: pluginList.width
                                height: pluginList.contentWidth > pluginList.width? implicitHeight: 0
                                orientation: Qt.Horizontal
                            }
                        }
                    }
                }
            }
        }
        Item {
            width: impl.contentWidth
            height: okButton.height
            Row {
                anchors.right: parent.right
                spacing: 5
                Button {
                    id: okButton
                    text: Constants.okTextWithMnemonic
                    onClicked: {
                        // TODO
                        root.close();
                    }
                }
                Button {
                    id: cancelButton
                    text: Constants.cancelTextWithMnemonic
                    onClicked: {
                        root.close();
                    }
                }
            }
        }
    }
}
