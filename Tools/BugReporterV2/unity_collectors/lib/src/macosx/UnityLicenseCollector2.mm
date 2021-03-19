#import <Foundation/Foundation.h>

#include <string>

namespace ureport
{
namespace unity_collectors
{
namespace macosx
{
    std::string GetApplicationSupportDirectoryPath()
    {
        id pool = [[NSAutoreleasePool alloc] init];
        id paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,
            NSLocalDomainMask, YES);
        const std::string path([[paths objectAtIndex: 0] UTF8String]);
        [pool release];
        return path;
    }
}
}
}
