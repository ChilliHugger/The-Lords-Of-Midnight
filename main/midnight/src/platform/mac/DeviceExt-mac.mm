//
//  DeviceExt-mac.m
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "DeviceExt-mac.h"

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
    }
}
