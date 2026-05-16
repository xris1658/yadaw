import QtQml.Models

ListModel {
    enum Role {
        NativePopup,
        ShouldReceiveKeyEvents,
        RoleCount
    }
    dynamicRoles: true
    function insert(nativePopup, index, shouldReceiveKeyEvents) {
        if(index >= 0 && index <= count && nativePopup) {
            insert(
                index,
                {
                    "npefm_native_popup": nativePopup,
                    "npefm_should_receive_key_events": shouldReceiveKeyEvents
                }
            );
            return true;
        }
        return false;
    }
    function append(nativePopup, shouldReceiveKeyEvents) {
        if(nativePopup) {
            append(
                {
                    "npefm_native_popup": nativePopup,
                    "npefm_should_receive_key_events": shouldReceiveKeyEvents
                }
            );
        }
    }
    function popupShouldReceiveKeyEvents(nativePopup, shouldReceiveKeyEvents) {
        for(let i = 0; i < count; ++i) {
            if(get(i).npefm_native_popup == nativePopup) {
                setProperty(i, "npefm_should_receive_key_events", shouldReceiveKeyEvents);
                return;
            }
        }
    }
    function remove(nativePopup) {
        for(let i = 0; i < count; ++i) {
            if(get(i).npefm_native_popup == nativePopup) {
                remove(i, 1);
                return true;
            }
        }
        return false;
    }
}