//
//  TunnelView.h
//  midnight
//
//  Created by Chris Wild on 13/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "ILandscape.h"

class TunnelView : public ILandscape
{
private:
    TunnelView();
    
public:
    static TunnelView* create( LandscapeOptions* options );
    
protected:
    bool initWithOptions( LandscapeOptions* options );
    
    void createColumns();
    void createSteps();
    void createColumn( f32 x, LPCSTR images[], u32 count, bool graffiti = false );
    
    Node* clipping;
};

