//
//  uithinkpage.cpp
//  midnight
//
//  Created by Chris Wild on 16/10/2018.
//Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "uithinkpage.h"
#include "../system/moonring.h"
#include "../system/configmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/tmemanager.h"
#include "../ui/uihelper.h"

#if defined(_LOM_)
#define NAME_X              32
#define NAME_Y              32
#define TEXT_X              32
#define TEXT_Y              332
#define TERRAIN_X           764 //512
#define TERRAIN_Y           352
#define CHARACTER_X         384
#define CHARACTER_Y         352 //332
#define OBJECT_Y            352 //332
#define OBJECT_X            128
#define BACKGROUND_COLOUR   _clrWhite
#define NAME_TEXT_COLOUR    _clrDarkRed
#endif
#if defined(_DDR_)
#define NAME_X              32
#define NAME_Y              32
#define TEXT_X              32
#define TEXT_Y              16
#define TERRAIN_X           640 //512
#define TERRAIN_Y           (768-96)
#define CHARACTER_X         190
#define CHARACTER_Y         (768-96)
#define OBJECT_Y            352 //332
#define OBJECT_X            128
#define BACKGROUND_COLOUR   _clrCyan
#define NAME_TEXT_COLOUR    _clrBlue
#endif

using namespace tme;


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
    
    this->id = id;
    this->mode = mode;
    this->objectid = objectId;
    
    // get the character we are interested in
    TME_GetCharacter(current_character, id);
    TME_GetCharacterLocationInfo ( current_character );
    flags = location_flags;
    
    setupUI();
    
    auto size = this->getContentSize();
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
        scrollView->setEnabled(false);
        scrollView->setBounceEnabled(false);
        scrollView->setScrollBarEnabled(false);
    }
    
    scrollView->setInnerContainerSize( Size(size.width,height) );
    scrollView->setContentSize(Size(size.width,size.height));
    //scrollView->setInnerContainerPosition(Vec2(0,0));

    // name
    uihelper::AddTopLeft(scrollView,lblName,RES(NAME_X),RES(NAME_Y));

    // text
    uihelper::AddTopLeft(scrollView,lblDescription, RES(TEXT_X),textY);

    // character
    y = RES(CHARACTER_Y) - imgCharacter->getContentSize().height;
    x = RES(CHARACTER_X) - (imgCharacter->getContentSize().width/2);
    uihelper::PositionParentTopLeft(imgCharacter,x,y);
    
    // terrain
    y = RES(TERRAIN_Y) - imgTerrain->getContentSize().height;
    x = RES(TERRAIN_X) - imgTerrain->getContentSize().width/2;
    uihelper::PositionParentTopLeft(imgTerrain,x,y);

    // object
#if defined(_LOM_)
    y = RES(OBJECT_Y) - imgObject->getContentSize().height;
    x = RES(OBJECT_X) - (imgObject->getContentSize().width/2);
    uihelper::PositionParentTopLeft(imgObject,x,y);
#endif
    
    uihelper::AddTopLeft(this, scrollView);
    uihelper::FillParent(scrollView);
    
}

bool uithinkpage::init()
{
    if ( !uielement::init() )
    {
        return false;
    }

    auto size = Director::getInstance()->getVisibleSize();
    
    setContentSize(size);
    
    scrollView = ui::ScrollView::create();
    scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);

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
    //lblDescription->setHeight(size.height-RES(TEXT_Y+64));
    lblDescription->setAlignment(TextHAlignment::LEFT);
    //lblDescription->setLineHeight(RES(FONT_SIZE_BIG));
    lblDescription->setLineSpacing(0);
    lblDescription->setMaxLineWidth(size.width-RES(TEXT_X*2));
    lblDescription->enableWrap(true);
    

    // Character Image
    imgCharacter = Sprite::create();

//    auto listener1 = EventListenerTouchOneByOne::create();
//    listener1->onTouchEnded = [](Touch* touch, Event* event){
//        return true; // if you are consuming it
//    };
//    imgCharacter->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, this);
//
    scrollView->addChild(imgCharacter);

    // Object Image
#if defined(_LOM_)
    // create image for object
    imgObject = Sprite::create();
    scrollView->addChild(imgObject);
#endif
    
    auto mr = moonring::mikesingleton();
    
    
    // Terrain Image
    imgTerrain = Sprite::create();
    imgTerrain->setGLProgramState(mr->glProgramState->clone());
    imgTerrain->setScale(1.0f);
    imgTerrain->setBlendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
    
    auto tint2 = Color4F(_clrWhite);
    auto tint1 = Color4F(0,0,0.65,0);
    
    imgTerrain->getGLProgramState()->setUniformFloat("p_alpha", 1.0f);                    // alpha
    imgTerrain->getGLProgramState()->setUniformVec4("p_left", Vec4(tint1.r,tint1.g,tint1.b,tint1.a));      // outline
    imgTerrain->getGLProgramState()->setUniformVec4("p_right", Vec4(tint2.r,tint2.g,tint2.b,tint2.a));               // body
    
    
    scrollView->addChild(imgTerrain);
    
    return true;
}

void uithinkpage::displayCharacter ( const character& c )
{

    // display character name
    lblName->setString(current_character.longname);

    imgCharacter->initWithFile(GetCharacterImage(c));

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
    terrain_data_t*    d = (terrain_data_t*)TME_GetEntityUserData(MAKE_ID(INFO_TERRAININFO, terrain));
    if ( d == nullptr || strlen(d->file) == 0 ) {
        return;
    }

    imgTerrain->initWithSpriteFrameName((LPCSTR)d->file);

    imgTerrain->setVisible(true);
}

void uithinkpage::displayArmy ( void )
{
    auto* mr = moonring::mikesingleton();
    terrain_data_t* d = ((terrain_data_t*)mr->tme->terrain[0]);
    if ( d == nullptr || strlen(d->file) == 0 ) {
        return;
    }

    imgTerrain->initWithSpriteFrameName((LPCSTR)d->file);

    imgTerrain->setVisible(true);
    
}


void uithinkpage::displayObject ( mxid objectid )
{
#if defined(_LOM_)
    if ( objectid == IDT_NONE ) {
        imgObject->setVisible(false);
        return;
    }
    
    imgObject->initWithFile( GetObjectBig(objectid) );
    imgObject->setVisible(true);
#endif
    
}


void uithinkpage::setupUI()
{
    tme::CStrBuf     buffer(1024);
    LPCSTR      text=NULL;
    
    displayCharacter ( current_character );
    displayCharacterTerrain(current_character);
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
    if ( Character_IsDead(current_character) ) {
        objectid = current_character.killedby;
        text = TME_GetCharacterText(current_character,"CharDeath");
        lblDescription->setString(text);
        return;
    }
    
    text = NULL ;
    switch ( mode ) {
            
        case MODE_THINK:
#if defined(_LOM_)
            if ( Character_IsHidden(current_character) ) {
                text = TME_GetSystemString(current_character,SS_HIDDEN);
                unhide = true;
            }else if ( objectid ) {
                if ( flags.Is(lif_fight) ) {
                    text = TME_GetSystemString(current_character,SS_MUSTFIGHT);
                    fight=true;
                    displayObject(objectid);
                }else{
                    text = TME_GetSystemString(current_character,SS_THINGATLOCATION);
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
    
    chilli::lib::c_strcat ( buffer.GetAt(), TME_GetSystemString(current_character,SS_MESSAGE1) );
    
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
        if ( msg == SS_MESSAGE7 && location_flags.Is(lif_recruitchar) ) {
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
//    s32 min = (s32)variables::sv_stronghold_default_min ;
//    s32 max = (s32)variables::sv_stronghold_default_max ;
//    s32 char_max = 0;
//    s32 can_recruit = 0;
//
//
//    // RECRUIT SOLDIERS
//    if ( flags.Is(lif_recruitmen) ||  flags.Is(lif_guardmen) ) {
//        character&        c = TME_CurrentCharacter();
//
//        if ( s.location == c.location ) {
//
//            stronghold_updown->Value(s.total);
//
//            if ( s.type == UT_WARRIORS ) {
//                char_max = (s32)sv_character_max_warriors;
//                Character_Army(c.warriors, army);
//                stronghold_updown->id = ID_POSTRECRUIT_WARRIORS;
//            }
//
//            if ( s.type == UT_RIDERS ) {
//                char_max = (s32)sv_character_max_riders;
//                Character_Army(c.riders, army);
//                stronghold_updown->id = ID_POSTRECRUIT_RIDERS;
//            }
//
//            can_recruit = char_max - army.total ;
//            stronghold_updown->Max( MIN((s32)s.total+(s32)army.total,max)  );
//
//            s32 min1 = (s32)s.total-can_recruit ;
//            s32 min2 = MIN(min,(s32)s.total);
//            s32 min3 = MAX(min1,min2);
//
//            stronghold_updown->Min(min3);
//            stronghold_updown->ShowEnable();
//        }
//    }
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