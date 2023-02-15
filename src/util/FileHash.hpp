#ifndef YADAW_UTIL_FILEHASH
#define YADAW_UTIL_FILEHASH

#include <QString>

#include <vector>

namespace YADAW::Util
{
std::vector<char> fileHash(const QString& filePath);
}

#endif //YADAW_UTIL_FILEHASH
