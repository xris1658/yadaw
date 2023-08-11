#ifndef YADAW_SRC_UTIL_VECTORUTIL
#define YADAW_SRC_UTIL_VECTORUTIL

#include <vector>

namespace YADAW::Util
{
template<typename T>
void resizeVector(std::vector<T>& vec, std::size_t newSize)
{
    if(auto oldSize = vec.size(); oldSize < newSize)
    {
        for(auto i = oldSize; i < newSize; ++i)
        {
            vec.emplace_back();
        }
    }
    else if(oldSize > newSize)
    {
        vec.erase(vec.begin() + newSize, vec.end());
    }
}
}

#endif // YADAW_SRC_UTIL_VECTORUTIL
