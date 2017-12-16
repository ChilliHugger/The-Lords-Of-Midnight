//
//  panel_dedication.h
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//

#pragma once

#include "uipanel.h"

class panel_dedication : public uipanel
{
public:
    
    virtual bool init();
    
    CREATE_FUNC(panel_dedication);
    
    void NextPanel();
    void StartDedication();
protected:
    Node* image;
};


