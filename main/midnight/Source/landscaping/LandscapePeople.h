//
//  LandscapePeople.h
//  midnight
//
//  Created by Chris Wild on 17/10/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#ifndef LandscapePeople_hpp
#define LandscapePeople_hpp

#include <stdio.h>

#include "LandscapeNode.h"
#include "LandscapeGenerator.h"
#include "../ui/uielement.h"

typedef struct {
    bool        used;
    point       pos;
    f32         scale;
    std::string image;
} persontodraw_t ;


#define DEFAULT_PRINT_RIDERS            8
#define DEFAULT_PRINT_WARRIORS          8
#if defined(_LOM_)
#define DEFAULT_PRINT_DRAGONS           2
#endif
#if defined(_DDR_)
#define DEFAULT_PRINT_DRAGONS           4
#endif
#define DEFAULT_PRINT_OTHER             4
#define DEFAULT_PRINT_CHARACTER         1

#if defined(_DDR_)
#define MAX_DISPLAY_CHARACTERS          6
#define MAX_DISPLAY_CHARACTERS_TUNNEL   4
#else
#define MAX_DISPLAY_CHARACTERS          8
#endif

#define CHARACTER_COLUMN_WIDTH          128
#define MAX_ALLOWED_CHARACTER_WIDTH     128


using namespace chilli::types;

class LandscapePeople : public LandscapeNode
{
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;
    using Widget = ax::ui::Widget;
        
public:
    static LandscapePeople* create( LandscapeOptions* options );

    void Initialise( const character& c, mxdir_t dir );
    void Initialise();
   
    void clear();
    void stopAnim();
    
    void startSlideFromRight(s32 distance);
    void startSlideFromLeft(s32 distance);
    void startSlideOffLeft(s32 distance);
    void startSlideOffRight(s32 distance);
    void adjustMovement( f32 amount );

    void startFadeIn();
    void startFadeOut();
    
    void setCallback(const WidgetClickCallback& callback);

    
protected:
    LandscapePeople();
    
    bool initWithOptions( LandscapeOptions* options );
    Widget* add( std::string& image, int number );

public:    
    tme::loc_t      loc;
    s32             characters;
    persontodraw_t  columns[8];
    f32             amountmoved;
    f32             startlocationx;
    f32             startlocationy;
    f32             amountmoving;
    WidgetClickCallback callback;
};



#endif /* LandscapePeople_hpp */
