//
//  MyButton.h
//  midnight
//
//  Created by Chris Wild on 21/12/2017.
//
//

#pragma once
#include "../cocos.h"

class MyButton : public cocos2d::ui::Button
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

