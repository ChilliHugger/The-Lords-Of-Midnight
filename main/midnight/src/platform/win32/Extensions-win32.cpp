//
//  Extensions-win32.m
//  midnight
//
//  Created by Chris Wild on 17/12/2020.
//

#pragma comment(lib,"user32.lib")
#include "DeviceExt-win32.h"

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

        std::string getBuildNo()
        {
            return "";
        }
    }
}
