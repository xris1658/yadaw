#ifndef YADAW_SRC_UTIL_ROLLBACKABLEOPERATION
#define YADAW_SRC_UTIL_ROLLBACKABLEOPERATION

#include "util/OptionalUtil.hpp"

#include <coroutine>

namespace YADAW::Util
{
struct RollbackablePromise;
/*
Suppose you have a rollbackable operation as follows:
```cpp
void rollbackableOperation(const std::function<bool()>& shouldCommitCallback)
{
    // [1] prepare
    if(shouldCommitCallback && shouldCommitCallback())
    {
        // [2] commit
    }
    else
    {
        // [3] rollback
    }
}
```
You can update this function to a coroutine as follows:
```cpp
RollbackableOperation rollbackableOperation()
{
    // [1] prepare
    if(bool shouldCommit = false; co_yield shouldCommit, shouldCommit)
    {
        // [2] commit
    }
    else
    {
        // [3] rollback
    }
}
```
This is for scenarios when you want to call a rollbackable operation without
moving your caller code modifying `shouldCommit` to a separate callback, i.e.
your caller can be as tidy as this:
```cpp
auto operation = rollbackableOperation();
auto shouldCommit = false;
// [4] Snippet of code modifying `shouldCommit`
operation.resume(shouldCommit);
```
instead of some potential callback hell:
```cpp
rollbackableOperation([capturing things](){
    auto shouldCommit = false;
    // [4] Snippet of code modifying `shouldCommit`
    return shouldCommit;
});
```
*/
struct RollbackableOperation: std::coroutine_handle<RollbackablePromise>
{
public:
    using promise_type = RollbackablePromise;
public:
    void commit() { resume(true); }
    void rollback() { resume(false); }
    void resume(bool shouldCommit);
};
}

#endif // YADAW_SRC_UTIL_ROLLBACKABLEOPERATION
