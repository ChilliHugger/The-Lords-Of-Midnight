//
//  MyButton.h
//  midnight
//
//  Created by Chris Wild on 21/12/2017.
//
//

#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class MyButton : public ui::Button
{
public:
    
    static MyButton* create(const std::string& normalImage,
                            const std::string& selectedImage = "",
                            const std::string& disableImage = "",
                            TextureResType texType = TextureResType::LOCAL);
    
protected:
    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    
};

