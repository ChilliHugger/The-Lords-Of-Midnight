//
//  Extensions-wasm.cpp
//  midnight
//
#include "Extensions-wasm.h"

USING_NS_AX;

namespace chilli
{
    namespace extensions
    {
        std::string getVersion()
        {
            return WASM_APP_VERSION;
        }

        std::string getBuildNo()
        {
            return WASM_APP_BUILD_NO;
        }
    }
}
