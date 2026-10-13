#ifndef __PANEL_SPLASHSCREEN_H__
#define __PANEL_SPLASHSCREEN_H__

#include "../ui/uipanel.h"
#include <vector>

FORWARD_REFERENCE(progressmonitor);

class panel_splashscreen : public uipanel
{
    using DrawNode = ax::DrawNode;
    template<class T> using Vector = ax::Vector<T>;
    using LayerColor = ax::LayerColor;
public:
    panel_splashscreen();

    virtual bool init() override;
    virtual ~panel_splashscreen() override;
    
    CREATE_FUNC(panel_splashscreen);

protected:
    virtual void update(float delta) override;
    
    void updateProgress( f32 percent );
    void complete();

    void initSpectrumLoad( ax::Node* background, const char* filename );
    void updateSpectrumLoad( f32 delta );
    
protected:
    f32                 loadingWidth;
    f32                 loadingHeight;
    DrawNode*           loadingProgress;
    f32                 currentProgress{};
    long                startTime;
    progressmonitor*    progress;
    DrawNode*           loadingBars;
    bool                loadingComplete;

    // spectrum style loading effect
    DrawNode*           pixelCover{};
    std::vector<ax::Sprite*> attributeCells{};
    f32                 spectrumHeight{};
    f32                 spectrumWidth{};
    f32                 spectrumTop{};      // visible part of the image, in image coordinates
    f32                 spectrumBottom{};
    f32                 spectrumTime{};
    int                 pixelLinesLoaded{-1};
    int                 attributeCellsLoaded{};

    // text drawn into the image: white until the attribute sweep passes it, then its own colour
    struct bakedlabel {
        ax::Label*  label;
        ax::Color4B colour;
        int         row;
        bool        coloured;
    };
    std::vector<bakedlabel> bakedLabels{};
    std::vector<int>        attributeCellRows{};

    void bakeLabel( ax::Node* background, ax::Label* label, ax::Color4B colour );
};

#endif // __PANEL_SPLASHSCREEN_H__
