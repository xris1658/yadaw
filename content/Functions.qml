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
}