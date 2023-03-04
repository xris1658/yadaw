#include "AssetDirectoryController.hpp"

#include "model/AssetDirectoryListModelImpl.hpp"

namespace YADAW::Controller
{
YADAW::Model::AssetDirectoryListModel& appAssetDirectoryListModel()
{
    static YADAW::Model::AssetDirectoryListModelImpl ret;
    return ret;
}
}