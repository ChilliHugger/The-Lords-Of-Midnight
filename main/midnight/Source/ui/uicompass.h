//
//  uicompass.h
//  revenge
//
//  Created by Chris Wild on 02/04/2022.
//

#pragma once
#include "uielement.h"
#include "uipanel.h"

class uicompass : public ax::Node
{
    using DrawNode = ax::DrawNode;
    using Vec2 = ax::Vec2;
    using EventListenerTouchOneByOne = ax::EventListenerTouchOneByOne;
   
public:
    static uicompass* create( uipanel* parent );
    void setSelected(mxdir_t direction);
    mxdir_t getSelected();
    
    void Show(EventListenerTouchOneByOne* listener = nullptr);
    void Close();
    
    MXVoidCallback onCancel;
    MXVoidCallback onOk;

protected:
    bool initWithParent( uipanel* parent );
    Vec2 calcCirclePos ( f32 pos );
    void setMode(DrawNode* node);
    void clearCurrentFocus();
    void clearSelectedFocus();
    void addTouchListener(EventListenerTouchOneByOne* listener = nullptr);

    DrawNode* getItem( Vec2 pos );
    f32 getDistanceFromCentre( Vec2 pos, Node* node ) const;

private:
    int         selected;
    int         current;
    uipanel*    parent;
};


