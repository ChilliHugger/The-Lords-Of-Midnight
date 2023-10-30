#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "mocks/mocks.h"
#include "../src/cocos.h"


int tests_main(int argc, char *argv[])
{
    mocks::init();

#if defined(_LOM_)
    cocos2d::FileUtils::getInstance()->setDefaultResourceRootPath("/Users/chris/Projects/GitHub/The-Lords-Of-Midnight/main/midnight/mac-tests-build-lom/bin/midnight/Debug/midnight.app/Contents/Resources");
#endif

#if defined(_DDR_)
    cocos2d::FileUtils::getInstance()->setDefaultResourceRootPath("/Users/chris/Projects/GitHub/The-Lords-Of-Midnight/main/midnight/mac-tests-build-ddr/bin/revenge/Debug/revenge.app/Contents/Resources");
#endif

    return Catch::Session().run( argc, argv );

}
