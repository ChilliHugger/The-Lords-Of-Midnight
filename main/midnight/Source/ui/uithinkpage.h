//
//  uithinkpage.h
//  midnight
//
//  Created by Chris Wild on 16/10/2018.
//Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#pragma once

#include "uielement.h"
#include "../frontend/panel_id.h"
#include "../tme/tme_interface.h"
#include "uishortcutkeys.h"

FORWARD_REFERENCE(moonring);

enum SpecialId
{
    None,
    ArmiesHere,
    ArmiesAhead
};


class uithinkpage :
    public chilli::ui::Element,
    public uishortcutkeys
            // , public uinotificationdelegate
            // , public uiactioncompletedelegate
            // , public uidragmoveelement
{
    using Sprite = cocos2d::Sprite;
    using ImageView = cocos2d::ui::ImageView;
    using ScrollView = cocos2d::ui::ScrollView;
    using Label = cocos2d::Label;
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;

public:
   
    CREATE_FUNC(uithinkpage);
    
    void setObject( mxid id, mxid objectId, panelmode_t mode );
    void setCallback( WidgetClickCallback callback ) { clickCallback = callback;}
    
    mxid Id() const { return id; }
    mxid ObjectId() const { return objectid; }
    
protected:

    uithinkpage();
    virtual bool init() override;
    
    void setupUIElements();
    void setImageColour(Sprite* node);
    
    void displayCharacter ( const character& c );
    void displayCharacterTerrain(  const character& c );
    void displayTerrain ( mxterrain_t terrain );
    void displayObject ( mxid objectid );
    void displayArmy ();
    
    void aheadOrHere( std::string& text, tme::loc_t location, bool aheaddir );
    void checkPerson();
    void checkPlace();
    void checkBattle();
    void checkNormal();
    void checkArmy();
        
    std::string checkFightAvailable();
    
    void recruitPostOptions ( stronghold& s );
    
protected:
    mxid                id;
    mxid                objectid ;
    panelmode_t         mode;
    //character           current_character;
    stronghold          current_stronghold;
    
    Label*              lblName;
    Label*              lblDescription;

    Sprite*             imgTerrain;
    ImageView*          imgCharacter;
    ImageView*          imgObject;
    
    ScrollView*         scrollView;
    Layer*              safeArea;
    WidgetClickCallback clickCallback;
    
public:
    moonring*           mr;
    bool                approach;
    bool                fight;
    bool                unhide;
    bool                leave;
    bool                disband;
    bool                postMen;
    bool                recruitMen;
    bool                enterTunnel;
};



