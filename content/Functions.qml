pragma Singleton

import QtQml
import QtQuick

QtObject {
    function arrayFromOne(count) {
        let ret = [];
        for(let i = 0; i < count; ++i) {
            ret.push(i + 1);
        }
        return ret;
    }
}
