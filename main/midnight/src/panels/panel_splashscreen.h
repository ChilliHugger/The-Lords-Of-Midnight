#ifndef __PANEL_SPLASHSCREEN_H__
#define __PANEL_SPLASHSCREEN_H__

#include "../ui/uipanel.h"

FORWARD_REFERENCE(progressmonitor);

class panel_splashscreen : public uipanel
{
    using DrawNode = cocos2d::DrawNode;
    template<class T> using Vector = cocos2d::Vector<T>;
    using LayerColor = cocos2d::LayerColor;
public:
    panel_splashscreen();

    virtual bool init() override;
    virtual ~panel_splashscreen() override;
    
    CREATE_FUNC(panel_splashscreen);

protected:
    virtual void update(float delta) override;
    
    void updateProgress( f32 percent );
    void complete();
    
protected:
    f32                 loadingWidth;
    f32                 loadingHeight;
    DrawNode*           loadingProgress;
    f32                 currentProgress;
    long                startTime;
    progressmonitor*    progress;
    DrawNode*           loadingBars;
    bool                loadingComplete;
};

#endif // __PANEL_SPLASHSCREEN_H__
