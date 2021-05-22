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
    virtual void onExit() override;
    
    void removeLoadingBars();
    void hideLoadingBars();


    void updateProgress( f32 percent );
    void complete();
    
protected:
    f32                 loading_width;
    f32                 loading_height;
    DrawNode*           loading_progress;
    long                StartTime;
    progressmonitor*    progress;
    Vector<LayerColor*> loading_bars;
    bool                loading_complete;
};

#endif // __PANEL_SPLASHSCREEN_H__
