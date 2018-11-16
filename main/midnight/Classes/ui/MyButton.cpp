//
//  MyButton.cpp
//  midnight
//
//  Created by Chris Wild on 21/12/2017.
//
//

#include "MyButton.h"



MyButton* MyButton::create(const std::string &normalImage,
                           const std::string& selectedImage ,
                           const std::string& disableImage,
                           TextureResType texType)
{
    MyButton *btn = new (std::nothrow) MyButton;
    if (btn && btn->init(normalImage,selectedImage,disableImage,texType))
    {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

void MyButton::onPressStateChangedToNormal()
{
    Button::onPressStateChangedToNormal();
    if ( getChildren().size() > 0 )
        getChildren().front()->setScale(1.0);
}

void MyButton::onPressStateChangedToPressed()
{
    Button::onPressStateChangedToPressed();
    if ( getChildren().size() > 0 )
        getChildren().front()->setScale(1.1f);
    
}
