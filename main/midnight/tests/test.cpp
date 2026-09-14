#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include "mocks/mocks.h"
#include "../Source/axmol_sdk.h"
#include "../Source/platform/Extensions.h"


int tests_main(int argc, char *argv[])
{
    mocks::init();

    // resolve the running test binary's own bundle Resources folder rather than
    // a hardcoded path, since the build output location varies by checkout/build config
    auto path = chilli::extensions::getBundleResourcePath();

#if defined(_CITADEL_)
    auto path = "/Users/chris/Projects/GitHub/The-Lords-Of-Midnight/main/midnight/Builds/mac-tests-citadel/build/Debug/citadel.app/Contents/Resources";
#endif

    ax::FileUtils::getInstance()->setDefaultResourceRootPath(path);
    return Catch::Session().run( argc, argv );

}
