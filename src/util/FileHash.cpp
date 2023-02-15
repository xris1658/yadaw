#include "FileHash.hpp"

#include <QCryptographicHash>
#include <QFile>

namespace YADAW::Util
{
std::vector<char> fileHash(const QString& filePath)
{
    QFile file(filePath);
    if(file.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::ExistingOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
        hash.addData(&file);
        auto result = hash.result();
        std::vector<char> ret(result.size());
        for(int i = 0; i < ret.size(); ++i)
        {
            ret[i] = result[i];
        }
        return ret;
    }
    return {};
}
}