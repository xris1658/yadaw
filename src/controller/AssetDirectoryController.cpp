#include "AssetDirectoryController.hpp"

namespace YADAW::Controller
{
YADAW::Model::AssetDirectoryListModel& appAssetDirectoryListModel()
{
    static YADAW::Model::AssetDirectoryListModel ret;
    return ret;
}
}