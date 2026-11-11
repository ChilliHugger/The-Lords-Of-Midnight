#include "../Source/AppDelegate.h"
#include "axmol.h"

USING_NS_AX;

int main(int argc, char* argv[])
{
#if defined(_UNIT_TESTS_)
    extern int tests_main(int argc, char* argv[]);
    return tests_main(argc, argv);
#else
    AppDelegate app;
    return Application::getInstance()->run();
#endif
}
