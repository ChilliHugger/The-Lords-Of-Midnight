#ifndef __PANEL_SPLASHSCREEN_H__
#define __PANEL_SPLASHSCREEN_H__

#include "uipanel.h"

FORWARD_REFERENCE(progressmonitor);

class panel_splashscreen : public uipanel
{
public:
    
    virtual bool init();
    
    CREATE_FUNC(panel_splashscreen);

    void UpdateProgress( f32 percent );
    void Complete();
    
protected:
    f32 loading_width;
    f32 loading_height;
    cocos2d::DrawNode* loading_progress;
    
    long StartTime;
    progressmonitor*    progress;
    
};

#endif // __PANEL_SPLASHSCREEN_H__
