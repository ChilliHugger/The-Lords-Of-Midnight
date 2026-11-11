//
//  Extensions-linux.cpp
//  midnight
//
#include "Extensions-linux.h"

#include <filesystem>

USING_NS_AX;

namespace chilli
{
    namespace extensions
    {
        void getDesktopSize(int& width, int& height)
        {
            // asked before the game window exists, so GLFW may not be up yet
            glfwInit();
            auto monitor = glfwGetPrimaryMonitor();
            auto mode = monitor != nullptr ? glfwGetVideoMode(monitor) : nullptr;
            width = mode != nullptr ? mode->width : 1024;
            height = mode != nullptr ? mode->height : 768;
        }

        std::string getVersion()
        {
            return LINUX_APP_VERSION;
        }

        std::string getBuildNo()
        {
            return LINUX_APP_BUILD_NO;
        }

        std::string getBundleResourcePath()
        {
            // resources are linked to a Content folder beside the executable
            return (std::filesystem::canonical("/proc/self/exe").parent_path() / "Content").string();
        }
    }
}
