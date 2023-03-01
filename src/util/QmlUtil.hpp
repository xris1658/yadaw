#ifndef YADAW_SRC_UTIL_QMLUTIL
#define YADAW_SRC_UTIL_QMLUTIL

#include <qqml.h>

#define QML_REGISTER_TYPE(class_name, module_name, major_version, minor_version) \
    qmlRegisterType<class_name>(#module_name, major_version, minor_version, #class_name)

#endif //YADAW_SRC_UTIL_QMLUTIL
