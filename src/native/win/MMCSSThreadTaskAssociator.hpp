#ifndef YADAW_SRC_NATIVE_WIN_MMCSSTHREADTASKASSOCIATOR
#define YADAW_SRC_NATIVE_WIN_MMCSSTHREADTASKASSOCIATOR

#if _WIN32

#include <windows.h>

namespace YADAW::Native
{
class MMCSSThreadTaskAssociator
{
public:
    MMCSSThreadTaskAssociator();
    ~MMCSSThreadTaskAssociator();
private:
    HANDLE handle_;
    DWORD taskIndex_;
};
}

#endif

#endif // YADAW_SRC_NATIVE_WIN_MMCSSTHREADTASKASSOCIATOR