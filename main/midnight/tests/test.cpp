#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

#include <cstdio>

#include "mocks/mocks.h"
#include "../Source/axmol_sdk.h"
#include "../Source/platform/Extensions.h"


// Announces each SCENARIO right before it runs, and flushes immediately, so
// a crash (which skips normal reporter output entirely) still leaves behind
// the name of whichever test was executing when it died.
struct ProgressListener : Catch::TestEventListenerBase
{
    using TestEventListenerBase::TestEventListenerBase;

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        printf("RUNNING: %s\n", testInfo.name.c_str());
        fflush(stdout);
    }

    void sectionStarting(Catch::SectionInfo const& sectionInfo) override
    {
        TestEventListenerBase::sectionStarting(sectionInfo);
        printf("%s\n", sectionInfo.name.c_str());
        fflush(stdout);
    }
};
CATCH_REGISTER_LISTENER(ProgressListener)


int tests_main(int argc, char *argv[])
{
    // When stdout is piped (e.g. CI redirecting to a log file) rather than a
    // terminal, the C runtime defaults to full block buffering - meaning a
    // crash (SIGSEGV etc.) discards whatever hasn't filled a buffer yet,
    // losing the trace of which test was running. Line-buffering trades a
    // little throughput for that output surviving a crash.
    setvbuf(stdout, nullptr, _IOLBF, 0);

    mocks::init();

    // resolve the running test binary's own bundle Resources folder rather than
    // a hardcoded path, since the build output location varies by checkout/build config
    auto path = chilli::extensions::getBundleResourcePath();

    ax::FileUtils::getInstance()->setDefaultResourceRootPath(path);
    return Catch::Session().run( argc, argv );

}
