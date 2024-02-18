//
//  Extensions-mac.m
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "Extensions-mac.h"

namespace chilli
{
    namespace extensions
    {
        void getDesktopSize(int& width, int& height)
        {
            NSScreen *screen = [NSScreen mainScreen];
            width = [screen frame].size.width;
            height = [screen frame].size.height;
        }

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
