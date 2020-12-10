//
//  uithinkpage.cpp
//  midnight
//
//  Created by Chris Wild on 16/10/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "../ui/uihelper.h"
#include "uithinkpage.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/tmemanager.h"
#include "../system/shadermanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/keyboard_id.h"

#if defined(_LOM_)
#define NAME_X              32
#define NAME_Y              32
#define TEXT_X              32
#define TEXT_Y              332
#define TERRAIN_X           (1024-764)
#define TERRAIN_Y           352
#define CHARACTER_X         384
#define CHARACTER_Y         352 //332
#define OBJECT_Y            352 //332
#define OBJECT_X            128
#define BACKGROUND_COLOUR   _clrWhite
#define NAME_TEXT_COLOUR    _clrDarkRed
#define TERRAIN_COLOUR      Color3B(0x00,0x00,0xA5)

#endif
#if defined(_DDR_)
#define NAME_X              32
#define NAME_Y              32
#define TEXT_X              32
#define TEXT_Y              16
#define TERRAIN_X           (1024-640)
#define TERRAIN_Y           (768-96)
#define CHARACTER_X         190
#define CHARACTER_Y         (768-96)
#define OBJECT_Y            352 //332
#define OBJECT_X            128
#define BACKGROUND_COLOUR   _clrCyan
#define NAME_TEXT_COLOUR    _clrBlue
#define TERRAIN_COLOUR      _clrBlack
#endif

using namespace tme;

USING_NS_CC;

uithinkpage::uithinkpage()
{
    approach = false;
    fight = false;
    unhide = false;
    leave = false;
    disband = false;
    postMen = false;
    recruitMen = false;
    id = IDT_NONE;
    objectid = IDT_NONE;
}

void uithinkpage::setObject( mxid id, mxid objectId, panelmode_t mode )
{
    f32 x,y;
    Vec2 pos = Vec2::ZERO;
    
    this->id = id;
    this->mode = mode;
    this->objectid = objectId;
    
    imgTerrain->removeAllChildren();
    
    setupUIElements();
    
    auto size = safeArea->getContentSize();
    auto textSize = lblDescription->getContentSize();
    
    f32 padding = RES(64);
    f32 textY = RES(TEXT_Y+64);
    f32 height = textY+textSize.height;

    if ( height >= size.height ) {
        height+=padding;
        scrollView->setBounceEnabled(true);
        scrollView->setScrollBarEnabled(true);
    }else{
        height=size.height;
        //scrollView->setEnabled(false);
        scrollView->setBounceEnabled(false);
        scrollView->setScrollBarEnabled(false);
    }
    scrollView->setSwallowTouches(false);
    scrollView->setInnerContainerSize( Size(size.width,height) );
    scrollView->setContentSize(Size(size.width,size.height));

    // name
    uihelper::AddTopLeft(scrollView,lblName,RES(NAME_X),RES(NAME_Y));

    // text
    uihelper::AddTopLeft(scrollView,lblDescription, RES(TEXT_X),textY);

    // character
    y = RES(CHARACTER_Y) - imgCharacter->getContentSize().height;
    x = RES(CHARACTER_X) - (imgCharacter->getContentSize().width/2);
    uihelper::PositionParentTopLeft(imgCharacter,x,y);
  
    
    //
    // Approach, Unhide, leave, disband
    // all show in the same place near the character
    //
    pos = imgCharacter->getPosition();
    pos.y -= imgCharacter->getContentSize().height - RES(16);
#if defined(_DDR_)
    pos.x += imgCharacter->getContentSize().width;
#endif
    
    
    // Approach
    auto approach = uihelper::CreateImageButton("i_approach", ID_APPROACH, clickCallback);
    approach->setPosition(pos);
    approach->setVisible(this->approach);
    scrollView->addChild(approach);
    
#if defined(_LOM_)
    // Unhide
    auto unhide = uihelper::CreateImageButton("i_unhide", ID_UNHIDE, clickCallback);
    unhide->setPosition(pos);
    unhide->setVisible(this->unhide);
    scrollView->addChild(unhide);
#endif
    
    // Leave
    auto leave = uihelper::CreateImageButton("i_leave_group", ID_GROUP_LEAVE, clickCallback);
    leave->setPosition(pos);
    leave->setVisible(this->leave);
    scrollView->addChild(leave);
    
    // Disband
    auto disband = uihelper::CreateImageButton("i_disband_group", ID_GROUP_DISBAND, clickCallback);
    disband->setPosition(pos);
    disband->setVisible(this->disband);
    scrollView->addChild(disband);
    
    // bottom gradient on character
   if ( this->unhide || this->approach || this->disband || this->leave ) {
       pos = imgCharacter->getPosition();
       pos.y -= imgCharacter->getContentSize().height;
       auto color = _clrWhite ;
       auto gradientB = uihelper::createVerticalGradient(color, RES(64), RES(56), RES(128), 1);
       gradientB->setPosition(pos);
       scrollView->addChild(gradientB);
    }
    
    // terrain
    y = RES(TERRAIN_Y) - imgTerrain->getContentSize().height;
    x = RES(TERRAIN_X) - imgTerrain->getContentSize().width/2;
    uihelper::PositionParentTopRight(imgTerrain,x,y);
    
    
    // Post/Recruit
    if ( this->recruitMen||this->postMen ) {
        auto gradientC = LayerGradient::create( Color4B(_clrWhite,ALPHA(0.0f)), Color4B(_clrWhite,ALPHA(1.0f)) );
        gradientC->setContentSize(Size(imgTerrain->getContentSize().width,RES(64)));
        imgTerrain->addChild(gradientC);
        uihelper::PositionParentBottomLeft(gradientC,RES(0),RES(0));
    }
    
    y = RES(TERRAIN_Y) ;
    x = RES(TERRAIN_X) ;
   
    // RECRUIT SOLDIERS
    auto recruitMen = uihelper::CreateImageButton("i_recruit", ID_RECRUITMEN, clickCallback);
    recruitMen->setVisible(this->recruitMen||this->postMen);
    recruitMen->setEnabled(this->recruitMen);
    recruitMen->setOpacity(this->recruitMen ? ALPHA(1.0) : ALPHA(0.25));
    recruitMen->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    imgTerrain->addChild(recruitMen);
    uihelper::PositionParentBottomCenter(recruitMen,
                                         -(recruitMen->getContentSize().width/2)-RES(8),
                                         -recruitMen->getContentSize().height/2);
   
    // POST SOLDIERS
    auto postMen = uihelper::CreateImageButton("i_post", ID_POSTMEN, clickCallback);
    postMen->setVisible(this->recruitMen||this->postMen);
    postMen->setEnabled(this->postMen);
    postMen->setOpacity(this->postMen ? ALPHA(1.0) : ALPHA(0.25));
    imgTerrain->addChild(postMen);
    uihelper::PositionParentBottomCenter(postMen,
                                         +(postMen->getContentSize().width/2)+RES(8),
                                         -(postMen->getContentSize().height/2)-RES(2));

    
    // object
#if defined(_LOM_)
    y = RES(OBJECT_Y) - imgObject->getContentSize().height;
    x = RES(OBJECT_X) - (imgObject->getContentSize().width/2);
    uihelper::PositionParentTopLeft(imgObject,x,y);

    auto fight = uihelper::CreateImageButton("i_fight", ID_FIGHT, clickCallback);
    pos = imgObject->getPosition();
    pos.y -= imgObject->getContentSize().height;
    fight->setPosition(pos);
    fight->setVisible(this->fight);
    scrollView->addChild(fight);
    
    if ( this->fight ) {
        auto gradientC = LayerGradient::create( Color4B(_clrWhite,ALPHA(0.0f)), Color4B(_clrWhite,ALPHA(1.0f)) );
        gradientC->setContentSize(Size(RES(128),RES(64)));
        gradientC->setPosition(pos);
        scrollView->addChild(gradientC);
    }
#endif

    uihelper::AddTopLeft(safeArea, scrollView);
    uihelper::FillParent(scrollView);
    
//#if defined(_LOM_)
//    addShortcutKey(ID_FIGHT         ,K_FIGHT);
//    addShortcutKey(ID_UNHIDE        ,K_UNHIDE);
//#endif
    
}

bool uithinkpage::init()
{
    if ( !Element::init() )
    {
        return false;
    }

    auto size = Director::getInstance()->getVisibleSize();
    setContentSize(size);
    
    auto padding = resolutionmanager::getInstance()->getSafeArea();
    size.width -= padding.left+padding.right;
    size.height -= padding.top+padding.bottom;
    safeArea = cocos2d::Layer::create();
    safeArea->setContentSize(size);
    safeArea->setLocalZOrder(ZORDER_FAR+1);
    safeArea->setPosition(padding.left,padding.bottom);
    addChild(safeArea);
    

    scrollView = ScrollView::create();
    scrollView->setDirection(ScrollView::Direction::VERTICAL);

    // Name
    lblName = Label::createWithTTF( uihelper::font_config_big, "" );
    lblName->getFontAtlas()->setAntiAliasTexParameters();
    lblName->setTextColor(Color4B(NAME_TEXT_COLOUR));
    lblName->setLocalZOrder(ZORDER_DEFAULT);
 
    // Description
    lblDescription = Label::createWithTTF( uihelper::font_config_big, "" );
    lblDescription->getFontAtlas()->setAntiAliasTexParameters();
    lblDescription->setTextColor(Color4B(_clrDarkRed));
    lblDescription->setLocalZOrder(ZORDER_DEFAULT);
    lblDescription->setWidth(size.width-RES(TEXT_X*2));
    lblDescription->setAlignment(TextHAlignment::LEFT);
    lblDescription->setLineSpacing(0);
    lblDescription->setMaxLineWidth(size.width-RES(TEXT_X*2));
    lblDescription->enableWrap(true);
    

    // Character Image
    imgCharacter = ImageView::create();

    scrollView->addChild(imgCharacter);

    // Object Image
#if defined(_LOM_)
    // create image for object
    imgObject = ImageView::create();
    scrollView->addChild(imgObject);
#endif
    
    // Terrain Image
    imgTerrain = Sprite::create();
    imgTerrain->setScale(1.0f);
    scrollView->addChild(imgTerrain);
    
    return true;
}

void uithinkpage::displayCharacter ( const character& c )
{

    // display character name
    lblName->setString(c.longname);

    imgCharacter->loadTexture(GetCharacterImage(c), cocos2d::ui::Widget::TextureResType::LOCAL);
    imgCharacter->setTouchEnabled( Character_IsRecruited(c) );
    imgCharacter->setTag(ID_SELECT_CHAR+c.id);
    imgCharacter->addClickEventListener(clickCallback);
    
    
}

void uithinkpage::displayCharacterTerrain(  const character& c )
{
#if defined(_DDR_)
    imgTerrain->setVisible(false);
    if ( Character_InTunnel(c) )
        return ;
#endif

    maplocation        map;
    TME_GetLocation( map,  MAKE_LOCID(c.location.x, c.location.y) );
    displayTerrain(map.terrain);
    
}

/*
 * Function name    : panel_think::DisplayTerrain
 *
 * Return type        : void
 *
 * Arguments        :
 *                  : terrain_t terrain
 *
 * Description        :
 *
 */

void uithinkpage::displayTerrain ( mxterrain_t terrain )
{
    terrain_data_t*    d = static_cast<terrain_data_t*>(TME_GetEntityUserData(MAKE_ID(INFO_TERRAININFO, terrain)));
    if ( d == nullptr || d->file.empty() ) {
        return;
    }

    imgTerrain->initWithSpriteFrameName(d->file);
    imgTerrain->setVisible(true);
    setImageColour(imgTerrain);

}

void uithinkpage::displayArmy ( void )
{
    auto* mr = moonring::mikesingleton();
    terrain_data_t* d = static_cast<terrain_data_t*>(mr->tme->terrain[0]);
    if ( d == nullptr || d->file.empty() ) {
        return;
    }

    imgTerrain->initWithSpriteFrameName(d->file);
    imgTerrain->setVisible(true);
    setImageColour(imgTerrain);
}

void uithinkpage::setImageColour(Sprite* node)
{
    auto mr = moonring::mikesingleton();
    auto tint2 = Color4F(_clrWhite);
    auto tint1 = Color4F(TERRAIN_COLOUR);
    mr->shader->AddTerrainTimeShader(node);
    mr->shader->UpdateTerrainTimeShader(node, alpha_normal,tint1,tint2);
}


void uithinkpage::displayObject ( mxid objectid )
{
#if defined(_LOM_)
    if ( objectid == IDT_NONE ) {
        imgObject->setVisible(false);
        return;
    }
    
    imgObject->loadTexture( GetObjectBig(objectid), cocos2d::ui::Widget::TextureResType::LOCAL );
    imgObject->setVisible(true);
#endif
    
}


void uithinkpage::setupUIElements()
{
    tme::CStrBuf     buffer(1024);
    LPCSTR      text=NULL;
    
    character& c = TME_CurrentCharacter();
    
    TME_GetCharacterLocationInfo ( c );
    flags = location_flags;
    
    displayCharacter (c);
    displayCharacterTerrain(c);
    displayObject(IDT_NONE);
    
    switch ( mode ) {
        case MODE_THINK_ARMY:
            checkArmy();
            return;
        case MODE_THINK_PERSON:
        case MODE_THINK_APPROACH:
            checkPerson();
            return;
        case MODE_THINK_PLACE:
            checkPlace();
            return;
        case MODE_THINK_BATTLE:
            checkBattle();
            return;
        default:
            break;
    }

    // completely different if dead!
    if ( Character_IsDead(c) ) {
        objectid = c.killedby;
        text = TME_GetCharacterText(c,"CharDeath");
        lblDescription->setString(text);
        return;
    }
    
    text = NULL ;
    switch ( mode ) {
            
        case MODE_THINK:
#if defined(_LOM_)
            if ( Character_IsHidden(c) ) {
                text = TME_GetSystemString(c,SS_HIDDEN);
                unhide = true;
            }else if ( objectid ) {
                if ( flags.Is(lif_fight) ) {
                    text = TME_GetSystemString(c,SS_MUSTFIGHT);
                    fight=true;
                    displayObject(objectid);
                }else{
                    text = TME_GetSystemString(c,SS_THINGATLOCATION);
                }
            }
#endif
            break;
            
        case MODE_THINK_RECRUITGUARD:
            text = TME_LastActionMsg();
            
            TME_GetStronghold(current_stronghold, location_strongholds[0]);
            recruitPostOptions(current_stronghold);
            lblDescription->setString(text);
            return;
            
            break;
            
        case MODE_THINK_SEEK:
        case MODE_THINK_FIGHT:
            text = TME_LastActionMsg();
#if defined(_DDR_)
            lblDescription->setString(text);
            return;
#endif
            break;
            
        default:
            text = NULL;
            break;
    }
    
    // TODO display entrance to tunnel
    
    if ( text ) {
        buffer.strcpy ( text );
    }
    
    chilli::lib::c_strcat ( buffer.GetAt(), TME_GetSystemString(c,SS_MESSAGE1) );
    
   // chilli::lib::c_strcat ( buffer.GetAt(), TME_GetSystemString(current_character,SS_MESSAGE1) );
   // chilli::lib::c_strcat ( buffer.GetAt(), TME_GetSystemString(current_character,SS_MESSAGE1) );
   // chilli::lib::c_strcat ( buffer.GetAt(), TME_GetSystemString(current_character,SS_MESSAGE1) );
   // chilli::lib::c_strcat ( buffer.GetAt(), TME_GetSystemString(current_character,SS_MESSAGE1) );
    
    text = buffer.GetAt();
    
    lblDescription->setString(text);
    
    
}

/*
 * Function name    : panel_think::AheadOrHere
 *
 * Return type        : void
 *
 * Arguments        : LPSTR text
 *                  : BOOL aheaddir
 *
 * Description        :
 *
 */

void uithinkpage::aheadOrHere ( LPSTR text, tme::loc_t location, bool aheaddir )
{
    character& c = TME_CurrentCharacter();
    
    if ( location.x == c.location.x && location.y == c.location.y) {
        sprintf ( text, "%s ", TME_GetSystemString(c, SS_PREFIX_HERE ) );
    }else{
        if ( aheaddir )
            sprintf ( text, "%s ", TME_GetSystemString(c, SS_PREFIX_AHEAD ) );
        else {
            sprintf ( text, "%s ", TME_GetSystemString(c, SS_PREFIX_DIRECTION ) );
            
        }
    }
    
}


/*
 * Function name    : panel_think::CheckPerson
 *
 * Return type        : void
 *
 * Arguments        : void
 *
 * Description        :
 *
 */
/*
 * LOM:
 * Here at the Tower of the Moon stands Corleth the Fey who
 * is loyal to the Fey.
 * Corleth is ....
 * Corleth's liege is Luxor the Moonprince.
 * Corleth's foe is Doomdark.
 
 * DDR:
 * Tarithel the Fey is good, strong, forceful, ..., and not reckless.
 * Tarithel is not at all despondent and is not at all tired.
 * Tarithel's liege is Luxor the Moonprince.
 * Tarithel's foe is Shareth the Heartstealer.
 
 * Here at the Tower of the Moon stands Corleth the Fey who
 * is loyal to the Fey.
 
 */

void uithinkpage::checkPerson ( void )
{
    character        c;
    tme::CStrBuf            text(512);
    int                msg=0;
    
    
    TME_GetCharacter(c,id) ;
    
#if defined(_LOM_)
    aheadOrHere ( text, c.location, TRUE );
#endif
    
    displayCharacterTerrain(c);
    displayCharacter ( c );
    
    if ( c.id == TME_CurrentCharacter().id ){
        msg = SS_MESSAGE3 ; // current character
    }else{
        
#if defined(_DDR_)
        aheadOrHere ( text, c.location, TRUE );
#endif
        
        msg = Character_IsRecruited(c) ? SS_MESSAGE4 : SS_MESSAGE7 ;
        
#if defined(_LOM_)
        if ( c.id == TME_GetId("CH_MIDWINTER", IDT_CHARACTER))
            msg=SS_MESSAGE8;
#endif
        bool isInBattle = Character_IsInBattle(c) ;
        
#if defined(_DDR_)
        if ( Character_IsPreparingForBattle(c) )
            isInBattle = TRUE ;
#endif
        
        // not recruited but we could recruit
        // then add an approach button
        if ( msg == SS_MESSAGE7 && flags.Is(lif_recruitchar) ) {
            if ( !isInBattle )
                approach = true;
        }
    }
    
    chilli::lib::c_strcat ( text, TME_GetSystemString(c, msg ) );
    
    
    if ( c.id == TME_CurrentCharacter().id ) {
        if ( Character_IsFollowing(c) ) {
            leave = true;
        }
        
        if ( Character_IsLeading(c) ) {
            disband = true;
        }
    }
    
    lblDescription->setString((LPCSTR)text);
    
}


/*
 * Function name    : panel_think::CheckPlace
 *
 * Return type        : void
 *
 * Arguments        : void
 *
 * Description        :
 *
 */

void uithinkpage::checkPlace ( void )
{
    CStrBuf            text(512);
    
#if defined(_DDR_)
    if ( ID_TYPE(id) ==  IDT_CHARACTER ) {
        checkNormal();
        return;
    }
#endif
    
    
    if ( id == 1 || id == 2 ) {
        checkArmy();
        return;
    }
    
    TME_GetStronghold( current_stronghold, id );
    aheadOrHere ( text, current_stronghold.location, TRUE );
    displayTerrain ( current_stronghold.terrain );
    
    variant args[2];
    args[0] = id;
    
    // seems a bit odd, not in the normal style of using an SS_ text token
    LPCSTR mytext = TME_GetText("Stronghold",args,1);
    sprintf ( text.End(), " %s", mytext );
    lblDescription->setString((LPCSTR)text);
    
    recruitPostOptions(current_stronghold);
    
}

void uithinkpage::recruitPostOptions ( stronghold& s )
{
    recruitMen = false;
    postMen = false;
    
    // RECRUIT SOLDIERS
    character&        c = TME_CurrentCharacter();

    if ( s.location == c.location ) {
        recruitMen = flags.Is(lif_recruitmen);
        postMen=flags.Is(lif_guardmen);
    }

}

/*
 * Function name    : panel_think::CheckBattle
 *
 * Return type        : void
 *
 * Arguments        : void
 *
 * Description        :
 *
 */

void uithinkpage::checkBattle ( void )
{
    CStrBuf            text(512);
    character&        c = TME_CurrentCharacter();
    
    displayCharacter ( c );
    displayCharacterTerrain(c);
    
    chilli::lib::c_strcat ( text, TME_GetCharacterText ( c, "CharBattle" ) );
    
    lblDescription->setString((LPCSTR)text);
    
}


/*
 * It is dawn. Luxor sees the sword of merlin and an underground entrance.
 * Here at the Gate of Varenorn, Luxor the Moonprince has found nothing.
 * Luxor carries nothing.
 *
 *
 *
 */

void uithinkpage::checkNormal ( void )
{
    CStrBuf            text(512);
    character        c;
    
    TME_GetCharacter(c,id) ;
    
    chilli::lib::c_strcat ( text, TME_GetSystemString(c,SS_MESSAGE1) );
    
    lblDescription->setString((LPCSTR)text);

}

/*
 * Function name    : panel_think::CheckArmy
 *
 * Return type        : void
 *
 * Arguments        : void
 *
 * Description        :
 *
 */
/*
 * Here at the Tower of the Moon stands Luxor the Moonprince.
 * Luxor commands no warriors.
 */

void uithinkpage::checkArmy ( void )
{
    CStrBuf            buffer(512);
    character        c;
    
    LPSTR text = buffer.GetAt();
    
    if ( id == 1 || id == 2 ) {
        int warriors,riders;
        
        
        mxid id = mxi->EntityByName( "CH_DOOMDARK", IDT_CHARACTER );
        TME_GetCharacter(c,id);
        
        if ( id == 1) {
            sprintf ( buffer.End(), "%s ", TME_GetSystemString(c, SS_PREFIX_HERE ) );
            displayArmy();
            warriors = location_armies.regiment_warriors;
            riders = location_armies.regiment_riders;
        }else{
            sprintf ( buffer.End(), "%s ", TME_GetSystemString(c, SS_PREFIX_AHEAD ) );
            warriors = location_infront_armies.regiment_warriors;
            riders = location_infront_armies.regiment_riders;
            displayArmy();
        }
        
        
        std::string strWarriors = TME_GetNumber(warriors);
        std::string strRiders = TME_GetNumber(riders);
        sprintf ( buffer.End(),
                 TME_GetSystemString(c,SS_WARRIORS_RIDERS),
                 strWarriors.c_str(),
                 strRiders.c_str()
                 );
        lblDescription->setString((LPSTR)text);
        return;
    }
    
    switch ( ID_TYPE(id) ) {
            
        case IDT_CHARACTER:
            
            TME_GetCharacter(c,id) ;
            aheadOrHere(text,c.location,TRUE);
            displayCharacter ( c );
            chilli::lib::c_strcat ( text, TME_GetSystemString(c, SS_MESSAGE2 ) );
            displayCharacterTerrain(c);
          
            if(c.id == TME_CurrentCharacter().id)
            {
                recruitPostOptions(location_stronghold);
            }
            break;
            
        case IDT_STRONGHOLD:
        {
            TME_GetStronghold(current_stronghold,id) ;
            aheadOrHere(text,current_stronghold.location,TRUE);
            displayTerrain ( current_stronghold.terrain );
            variant args[2];
            args[0] = current_stronghold.id ;
            chilli::lib::c_strcat( text, TME_GetText("Stronghold", args, 1 ) );
            
            recruitPostOptions(current_stronghold);
            
            break;
        }
        default:
            break;
    }
    
    lblDescription->setString((LPCSTR)text);
    
}
