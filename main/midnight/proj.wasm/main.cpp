#include "../Source/AppDelegate.h"
#include "axmol.h"

USING_NS_AX;

namespace
{
std::unique_ptr<AppDelegate> appDelegate;
}

void axmol_wasm_app_exit()
{
    appDelegate = nullptr;

#if AX_OBJECT_LEAK_DETECTION
    Object::printLeaks();
#endif
}

int main(int argc, char** argv)
{
    appDelegate.reset(new AppDelegate());
    return Application::getInstance()->run();
}
