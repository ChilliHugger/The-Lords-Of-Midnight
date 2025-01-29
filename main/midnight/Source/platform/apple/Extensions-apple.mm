//
//  Extensions-apple.m
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//

#import <Foundation/Foundation.h>

#include "Extensions-apple.h"
#include "../../axmol_sdk.h"

USING_NS_AX;

namespace chilli
{
    namespace extensions
    {
      std::string getBuildNo() {
          NSString* version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
          if (version) {
              return [version UTF8String];
          }
          return "";
      }

      std::string getVersion()
      {
          return Application::getInstance()->getVersion();
      }
    }
}
