//
//  Extensions-win32.m
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//

#pragma comment(lib,"user32.lib")
#include "Extensions-win32.h"
#include "../../cocos.h"

USING_NS_CC;

namespace chilli
{
    namespace extensions
    {
        void getDesktopSize(int& width, int& height)
        {
            RECT desktop;
            // Get a handle to the desktop window
            const HWND hDesktop = GetDesktopWindow();
            // Get the size of screen to the variable desktop
            GetWindowRect(hDesktop, &desktop);
            // The top left corner will have coordinates (0,0)
            // and the bottom right corner will have coordinates
            // (horizontal, vertical)
          	width=desktop.right;
          	height=desktop.bottom;
        }

        std::string getVersion() {
            std::string version = Application::getInstance()->getVersion();
            size_t buildLocation = version.find_last_of('.');
            return version.substr(0, buildLocation);
        }

        std::string getBuildNo()
        {
            std::string version = Application::getInstance()->getVersion();
            size_t buildLocation = version.find_last_of('.');
            return version.substr(buildLocation+1);
        }
    }
}
