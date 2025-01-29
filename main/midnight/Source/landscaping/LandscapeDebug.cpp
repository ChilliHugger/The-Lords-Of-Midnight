//
//  LandscapeDebug.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeView.h"
#include "LandscapeGenerator.h"
#include "LandscapeNode.h"
#include "LandscapeSky.h"
#include "LandscapeLand.h"
#include "LandscapeTerrain.h"
#include "LandscapeDebug.h"
#include "LandscapeColour.h"
#include "../system/moonring.h"


LandscapeDebug* LandscapeDebug::create( LandscapeOptions* options )
{
    LandscapeDebug* node = new (std::nothrow) LandscapeDebug();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}


bool LandscapeDebug::initWithOptions( LandscapeOptions* options )
{
    if ( !LandscapeNode::initWithOptions(options) )
        return false;

    return true;
}

void LandscapeDebug::Build()
{
    const float	horizonOffset = RES( (112*LANDSCAPE_GSCALE) );

    auto items  = options->generator->items;
    
    f32 height = getContentSize().height;
    f32 width = getContentSize().width;
    
    if ( options->debugMode != 0 ) {
        
        
        for(auto const& item: *items) {
            
            if ((item->position.z>=options->generator->viewportNear)&&(item->position.z<options->generator->viewportFar))
            {
                
                int x = options->generator->NormaliseXPosition(item->position.x);
                
                f32 y = item->position.y + horizonOffset ;
                
                // debug
                auto dot = Sprite::createWithSpriteFrameName( "dot" );
                dot->setColor(Color3B::RED);
                dot->setPosition(x, height - y);
                dot->setScale(0.1f);
                dot->setAnchorPoint(Vec2(0.5,0.5));
                this->addChild(dot);
                
                char buffer[20];
                
                switch (options->debugMode) {
                    case 1:
                        snprintf(buffer, NUMELE(buffer), "%f,%f", item->loc.x-(options->here.x/LANDSCAPE_DIR_STEPS), item->loc.y-(options->here.y/LANDSCAPE_DIR_STEPS));
                        break;
                    case 2:
                        snprintf(buffer, NUMELE(buffer), "%f", item->scale);
                        break;
                    case 3:
                        snprintf(buffer, NUMELE(buffer), "%d,%d", (int)item->position.x, (int)item->position.y);
                        break;
                    case 4:
                        snprintf(buffer, NUMELE(buffer), "%d,%d", item->loc.x, item->loc.y);
                        break;
                    case 5:
                        snprintf(buffer, NUMELE(buffer), "%d", item->terrain);
                        break;
                    default:
                        break;
                }
                
                
                auto label = Label::createWithTTF( buffer, "fonts/arial.ttf", 10);
                
                label->setColor(Color3B::BLUE);
                label->setAlignment(TextHAlignment::CENTER);
                label->setAnchorPoint(Vec2(0.5,0.5));
                label->setPosition(x, height - y - LRES(5));
                this->addChild(label, 1);
                
                //        auto dot1 = Sprite::createWithSpriteFrameName( "dot" );
                //        dot1->setColor(Color3B::BLUE);
                //        dot1->setPosition(x-(256*scale), height - y);
                //        dot1->setScale(0.1f);
                //        dot1->setAnchorPoint(Vec2(0.5,0.5));
                //        current_landscape_node->addChild(dot1);
                //
                //        auto dot2 = Sprite::createWithSpriteFrameName( "dot" );
                //        dot2->setColor(Color3B::RED);
                //        dot2->setPosition(x+(256*scale), height - y);
                //        dot2->setScale(0.1f);
                //        dot2->setAnchorPoint(Vec2(0.5,0.5));
                //        current_landscape_node->addChild(dot2);
                
            }
            
        }
        
    }
    
    
    
    char buffer[255];
    
    snprintf(buffer, NUMELE(buffer), "Location %d,%d - Looking %d Landscape Adj=(%f,%f) Rotating=%d"
            , options->currentLocation.x, options->currentLocation.y
            , options->currentDirection
            , options->landScaleX, options->landScaleY
            , (int)options->lookAmount
            );
    
    
    
    auto label = Label::createWithTTF( buffer, "fonts/arial.ttf", 20);
    
    label->setMaxLineWidth(width);
    label->setAnchorPoint(Vec2::ZERO);
    label->setColor(Color3B::WHITE);
    label->setPosition(LRES(10), (height - LRES(20)));
    this->addChild(label, 1);
    
    
}


