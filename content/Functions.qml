pragma Singleton

import QtQml
import QtQuick

QtObject {
    function parseStringAsInt(value) {
        if(/^(\-|\+)?([0-9]+|Infinity)$/.test(value)) {
            return Number(value);
        }
        return NaN;
    }
    function arrayFromOne(count) {
        let ret = [];
        for(let i = 0; i < count; ++i) {
            ret.push(i + 1);
        }
        return ret;
    }
}
