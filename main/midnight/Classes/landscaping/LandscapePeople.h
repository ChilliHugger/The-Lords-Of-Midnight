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

#include <stdio.h>

#include "LandscapeNode.h"
#include "LandscapeGenerator.h"

typedef struct {
    bool    used;
    point   pos;
    f32     scale;
    LPCSTR  image;
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




class LandscapePeople : public LandscapeNode
{
public:
    
    CREATE_FUNC(LandscapePeople);
    virtual bool init() override;
    
    virtual void Init(LandscapeOptions* options) override;
   
    void Initialise( const character& c);
    void Initialise( mxdir_t dir, bool isintunnel );
    void Initialise( bool isintunnel );
   
    void clear();
    void stopAnim();
    
    void startSlideFromRight();
    void startSlideFromLeft();
    void startSlideOffLeft();
    void startSlideOffRight();
    void adjustMovement( f32 amount );

    void startFadeIn();
    void startFadeOut();

    
protected:

    void add( LPCSTR image, int number /*, void* hotspot*/ );

public:
    GLProgramState* programState;
    
#if defined(_DDR_)
    bool            islookingdowntunnel;
    bool            isintunnel;
    mxtime_t        time;
#endif
    tme::loc_t      loc;
    s32             characters;
    persontodraw_t  columns[8];
    f32             amountmoved;
    f32             startlocationx;
    f32             startlocationy;
    f32             amountmoving;
};



#endif /* LandscapePeople_hpp */
