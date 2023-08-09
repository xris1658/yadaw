import QtQuick
import QtQuick.Layouts

ListView {
    id: root
    width: 128
    property int noteNameMark: Constants.NoteName.Sharp
    property double keyHeight: 14
    QtObject {
        id: impl
        function isBlackKey(index: int) {
            let indexInOctave = index % 12;
            return indexInOctave === 1
                || indexInOctave === 3
                || indexInOctave === 6
                || indexInOctave === 8
                || indexInOctave === 10
        }
    }
    height: count * keyHeight
    model: Constants.midiValueRange
    verticalLayoutDirection: ListView.BottomToTop
    property int keyRoot: 0
    enum ShowNoteName {
        Hide,
        ShowRoot,
        ShowAll
    }
    property int showNoteName: PianoKeys.ShowNoteName.ShowRoot
    delegate: Rectangle {
        width: root.width
        height: root.keyHeight
        color: impl.isBlackKey(index)? "#000000": "#FFFFFF"
        Label {
            anchors.right: parent.right
            anchors.rightMargin: 3
            text: Constants.dynamicKeyName.get(index % 12)[root.noteNameMark === Constants.NoteName.Sharp? "sharp": "flat"] + Math.floor(index / 12).toString()
            visible: root.showNoteName === PianoKeys.ShowNoteName.ShowAll || (root.showNoteName === PianoKeys.ShowNoteName.ShowRoot && index % 12 === root.keyRoot)
            color: impl.isBlackKey(index)? "#FFFFFF": "#000000"
        }
    }
}
