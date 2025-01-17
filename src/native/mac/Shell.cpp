#if __APPLE__

#include "native/Shell.hpp"

#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBase.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFBundle.h>
#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFString.h>

#include <mutex>

namespace YADAW::Native
{
QString fileBrowserName;

std::once_flag getFileBrowserNameFlag;

QString getFileBrowserName()
{
    std::call_once(getFileBrowserNameFlag,
        []()
        {
            auto finderAppPath = CFSTR("/System/Library/CoreServices/Finder.app");
            auto finderAppUrl = CFURLCreateWithFileSystemPath(
                kCFAllocatorDefault, finderAppPath, kCFURLPOSIXPathStyle, true
            );
            auto bundle = CFBundleCreate(kCFAllocatorDefault, finderAppUrl);
            if(bundle)
            {
                auto defaultName = CFSTR("Finder");
                auto infoPlist = CFSTR("InfoPlist");
                auto key = CFSTR("CFBundleDisplayName");
                auto name = CFBundleCopyLocalizedString(
                    bundle, key,
                    defaultName,
                    infoPlist
                );
                CFRelease(infoPlist);
                CFRelease(defaultName);
                CFRelease(key);
                fileBrowserName = QString::fromCFString(name);
                CFRelease(bundle);
            }
            else
            {
                fileBrowserName = "Finder";
            }
        }
    );
    return fileBrowserName;
}

void locateFileInExplorer(const QString& path)
{
    auto command = QString("open -R %1").arg(path).toLocal8Bit();
    std::system(command.constData());
}
}

#endif