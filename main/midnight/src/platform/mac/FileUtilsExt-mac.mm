#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <ftw.h>
#include <string>
#include <stack>

#include "FileUtilsExt-mac.h"

namespace chilli
{
    namespace extensions
    {
        std::string getApplicationSupportPath()
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
            NSString *documentsDirectory = [paths objectAtIndex:0];
            std::string strRet = [documentsDirectory UTF8String];
            strRet.append("/");
            return strRet;
        }
    }
}
