//#include "extensions/cocos-ext.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "MainMenuScene.h"

#include "CreditsScene.h"

#include "SimpleAudioEngine.h"
#include "resolutionmanager.h"

#include "../frontend/layout_id.h"
#include "../ui/uitextmenuitem.h"



USING_NS_CC;

using namespace cocos2d::ui;

#define KEYCODE(x) \
    cocos2d::EventKeyboard::KeyCode::KEY_##x


static uitextmenuitem items[] = {
    { ID_NEW_STORY,         {"NEW STORY"},     KEYCODE(N), "N" },
    { ID_CONTINUE_STORY,    {"CONTINUE STORY"},KEYCODE(C), "C" },
    { ID_END_STORY,         {"END STORY"},     KEYCODE(E), "E" },
    //#ifdef _USE_DEBUG_MENU_
    //    { ID_DEBUG_EMAIL,         {"TO SDCARD"},        KEYCODE(Z), "Z"},
    //#else
    { ID_CREDITS,           {"CREDITS"},        KEYCODE(Z), "Z"},
    //#endif
 
    { ID_OPTIONS,           {"OPTIONS"},        KEYCODE(O), "O"},

#if defined(_OS_DESKTOP_)
    { ID_EXIT,              {"EXIT" },          KEYCODE(ESCAPE), "ESC"},
#endif
    
#if defined(_USE_VERSION_CHECK_)
    { ID_UPDATE,            {"UPDATE" },        KEYCODE(U), "U"},
#endif
    
};



Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

bool MainMenu::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto p = Sprite::create("screens/misc/main_menu.png");
    addChild(p);

    p->setPosition(Vec2(0,0));
    p->setAnchorPoint(Vec2(0,0));

    float scaley =  ( visibleSize.height / p->getContentSize().height)  ;
    p->setScale(1.0, scaley );

    createMenu();
    
    return true;
}

void MainMenu::OnMenuItem( Ref* element, ICON_ID id )
{
    switch (id) {
#if defined(_OS_DESKTOP_)
        case ID_EXIT:
            OnExit();
            break;
#endif
#ifdef _USE_DEBUG_MENU_
        case ID_DEBUG_EMAIL:
            break;
#endif
#if defined(_USE_VERSION_CHECK_)
        case ID_UPDATE:
            OnUpdate();
            break;
#endif
        case ID_CREDITS:
            OnCredits();
            break;
        case ID_OPTIONS:
            OnOptions();
            break;
        case ID_HELP_CLOSE:
            OnHelpClose();
            break;
        case ID_MANUAL:
            OnShowManual();
            break;
        case ID_GUIDE:
            OnShowGuide();
            break;
        case ID_NEW_STORY:
            OnNewStory();
            break;
        case ID_CONTINUE_STORY:
            OnContinueStory();
            break;
        case ID_END_STORY:
            OnEndStory();
            break;
        default:
            break;
    }
    
    if ( id > ID_STORIES ) {
        
    }
    
}

void MainMenu::OnShowManual()
{
}

void MainMenu::OnShowGuide()
{
}

void MainMenu::OnHelpClose()
{
}

void MainMenu::OnOptions()
{
}

void MainMenu::OnCredits()
{
    auto scene = Credits::create();
    Director::getInstance()->pushScene(TransitionCrossFade::create(1.0, scene) );

}

void MainMenu::OnExit()
{
    Director::getInstance()->popScene();
}

void MainMenu::OnUpdate()
{
}

void MainMenu::OnNewStory()
{
}

void MainMenu::OnContinueStory()
{
}

void MainMenu::OnEndStory()
{
}

void MainMenu::createMenu()
{
    
    auto background = Scale9Sprite::create("misc/box_16.png");
    background->setPosition( Vec2(RES(1024/2),RES(768/2)) );
    background->setContentSize(Size(RES(512),RES(NUMELE(items)*RES(60))));
    background->setAnchorPoint( Vec2(0.5,0.5) );
    background->setColor(Color3B::BLACK);
    background->setOpacity(ALPHA(0.25));
    this->addChild(background);
    
    
    auto mainmenu = Menu::create();

    
    TTFConfig labelConfig;
    labelConfig.fontFilePath = "fonts/celtic.ttf";
    labelConfig.fontSize = RES(30);
    labelConfig.glyphs = GlyphCollection::DYNAMIC;
    labelConfig.outlineSize = 0;
    labelConfig.customGlyphs = nullptr;
    labelConfig.distanceFieldEnabled = false;

    //Vec2 offset = Vec2(RES(1024/2),RES(768-100));
    
    auto offset = Vec2( background->getBoundingBox().getMidX(), background->getBoundingBox().getMaxY() - RES(30));
    
    for ( auto item : items ) {
        
        auto label = Label::createWithTTF(labelConfig, item.type.text );
        label->getFontAtlas()->setAntiAliasTexParameters();
        label->setTextColor(Color4B::WHITE);
        //label->setLineHeight(RES(30));
        //label->setLineSpacing(0);
   
        auto menuItem = MenuItemLabel::create(label);
        menuItem->setPosition(offset);
        
        ICON_ID id = (ICON_ID)item.id;
        
        menuItem->setCallback([&,id](cocos2d::Ref *sender) {
            OnMenuItem( sender, id );
        } );
        
        mainmenu->addChild(menuItem);
        
        offset.y -= RES(60);
        
    }

    mainmenu->setPosition(Vec2::ZERO);
    

    
    
    this->addChild(mainmenu);
    
}


/*
 Button* button = Button::create("misc/box_16.png");
 button->setTouchEnabled(true);
 button->setTitleColor(Color3B::RED);
 button->setTitleFontSize(RES(30));
 button->setPosition(Vec2(RES(1024/2),RES(768-100)));
 button->setScale9Enabled(true);
 button->setContentSize( Size(RES(256),RES(120)) );
 button->setTitleText( item.type.text );
 button->setAnchorPoint(Vec2(0.5,0.5));
 button->setColor(Color3B::WHITE);
 button->setTitleAlignment(TextHAlignment::CENTER);
 
 button->setTitleFontName("fonts/celtic.ttf");
 auto label = button->getTitleRenderer();
 
 //label->setLineHeight(RES(30));
 //label->setLineSpacing(0);
 label->getFontAtlas()->setAntiAliasTexParameters();
 //   label->getFontAtlas()->setAliasTexParameters();
 
 button->addTouchEventListener( [&](Ref* s,Widget::TouchEventType e) {
 
 } );
 
 this->addChild(button);
 */



