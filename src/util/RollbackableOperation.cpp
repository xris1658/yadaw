#include "RollbackableOperation.hpp"

namespace YADAW::Util
{
void RollbackableOperation::resume(bool shouldCommit)
{
    if(auto optRefShouldCommit = promise().shouldCommit_)
    {
        optRefShouldCommit->get() = shouldCommit;
        this->operator()();
    }
}
}
