pragma Singleton

import QtQml
import QtQuick

Item {
    enum TextLook {
        MnemonicDisabled,
        MnemonicEnabled,
        MnemonicEnabledWithUnderline
    }
    function text(input: string) {
        let ret = input.indexOf('&') >= 0?
            input.split('&')[0] + input.split('&')[1].substring(0, 1) + input.split('&')[1].substring(1, input.split('&')[1].length):
            input;
        return ret;
    }
    function textWithUnderline(input: string) {
        let ret = input.indexOf('&') >= 0?
            input.split('&')[0] + "<u>" + input.split('&')[1].substring(0, 1) + "</u>" + input.split('&')[1].substring(1, input.split('&')[1].length):
            input;
        return ret;
    }
}
