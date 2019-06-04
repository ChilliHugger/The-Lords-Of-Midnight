//
//  uiselectpanel.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once

#include "../uielement.h"

class uiselectpanel : public chilli::ui::Element
{
public:
    CREATE_FUNC(uiselectpanel);
    virtual bool init() override;
    
protected:
    uiselectpanel();
};
