#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "mocks/mocks.h"
#include "../Source/cocos.h"


int tests_main(int argc, char *argv[])
{
    mocks::init();

#if defined(_LOM_)
    auto path = "/Users/chris/Projects/GitHub/The-Lords-Of-Midnight/main/midnight/Builds/mac-tests-lom/build/Debug/midnight.app/Contents/Resources";
#endif

#if defined(_DDR_)
    auto path = "/Users/chris/Projects/GitHub/The-Lords-Of-Midnight/main/midnight/Builds/mac-tests-ddr/build/Debug/revenge.app/Contents/Resources";
#endif

    cocos2d::FileUtils::getInstance()->setDefaultResourceRootPath(path);
    return Catch::Session().run( argc, argv );

}
