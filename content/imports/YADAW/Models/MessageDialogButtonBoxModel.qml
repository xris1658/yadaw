import QtQml.Models

ListModel {
    enum Role
    {
        ButtonRole,
        ButtonText,
        RoleCount
    }
    enum ButtonRoles
    {
        ButtonRoleOk,
        ButtonRoleCancel,
        ButtonRoleApply,
        ButtonRoleYes,
        ButtonRoleNo,
        ButtonRoleYesToAll,
        ButtonRoleNoToAll,
        ButtonRoleAbort,
        ButtonRoleRetry,
        ButtonRoleIgnore
    }
    dynamicRoles: true
    // Component.onCompleted: {
    //     append({
    //         "mdbbm_button_role": ButtonRoles.ButtonRoleOk,
    //         "mdbbm_button_text": "&OK"
    //     });
    //     append({
    //         "mdbbm_button_role": ButtonRoles.ButtonRoleCancel,
    //         "mdbbm_button_text": "&Cancel"
    //     });
    // }
}
