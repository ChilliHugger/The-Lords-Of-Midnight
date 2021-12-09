//
//  Extensions-ios.m
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//
#include "Extensions-ios.h"
#import <UIKit/UIKit.h>

namespace chilli
{
    namespace extensions
    {
        int getOrientation()
        {
            switch ([UIDevice.currentDevice orientation]) {
                case UIDeviceOrientationLandscapeLeft:
                    return 1; // notch left
                case UIDeviceOrientationLandscapeRight:
                    return 2; // notch right
                default:
                    return 0;
            }
        }
    }
}
