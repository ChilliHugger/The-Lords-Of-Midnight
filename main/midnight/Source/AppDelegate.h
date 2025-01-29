#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "axmol.h"

/**
@brief    The axmol Application.

Private inheritance here hides part of interface from Director.
*/
class  AppDelegate : private ax::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  Called when the application moves to the background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  Called when the application reenters the foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();
    
    
    void InitialisePaths();

#if defined(_OS_IOS_)
    virtual void applicationScreenSizeChanged(int newWidth, int newHeight);
#endif


};

#endif // _APP_DELEGATE_H_

