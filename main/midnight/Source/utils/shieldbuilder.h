//
//  shieldbuilder.h
//  midnight
//
//  Created by Chris Wild on 15/03/2025.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef shieldbuilder_h
#define shieldbuilder_h

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "../tme/tme_interface.h"
#include "../ui/uihelper.h"

USING_NS_TME;

enum class PartColour : int {
    Black,
    Blue,
    Red,
    Magenta,
    Green,
    Cyan,
    Yellow,
    White
};

enum class ShieldPart : int {
    BirdTop,
    BirdMiddle,
    BirdBottom,
    CatTop,
    CatMiddle,
    EyeTopLeft,
    EyeBottomRight,
    MoonCenter,
    MoonRight,
    MoonLeft,
    StarTopMiddle,
    StarTopLeft,
    StarMiddleLeft,
    StarBottom,
    Tear1,
    Tear2,
    Tear3,
    Tear4,
    Tear5,
};

typedef struct {
    LPCSTR name;
    f32 x;
    f32 y;
} ShieldPart_t ;

typedef struct  {
    ShieldPart part;
    PartColour colour;
} Part_t ;

typedef struct {
    std::string name;
    PartColour colour;
    std::vector<Part_t> parts;
} Shield_t ;

class shieldbuilder
{
public:
    using ImageView = ax::ui::ImageView;
public:
    shieldbuilder();
    virtual ~shieldbuilder();
    
    bool build(mxid id, ImageView* parent);
    
private:
    Shield_t* findShield(std::string& name);
    ImageView* AddPart(ImageView* node, ShieldPart part, PartColour colour);

public:

};


#endif /* shieldbuilder_h */
