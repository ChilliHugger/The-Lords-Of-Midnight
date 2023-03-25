#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "mocks.h"
#include "../src/cocos.h"


int tests_main(int argc, char *argv[])
{
    mocks::init();

    cocos2d::FileUtils::getInstance()->setDefaultResourceRootPath("/Users/chris/Projects/GitHub/The-Lords-Of-Midnight/main/midnight/mac-tests-build-lom/bin/midnight/Debug/midnight.app/Contents/Resources");
    
    return Catch::Session().run( argc, argv );

}
