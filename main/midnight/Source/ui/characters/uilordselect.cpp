//
//  uilordselect.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include <algorithm>
#include "../uihelper.h"
#include "uilordselect.h"
#include "../../utils/imagebuilder.h"
#include "../../tme/tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/moonring.h"
#include "../../system/resolutionmanager.h"

USING_NS_AX;

constexpr f32 LORD_FACE_SIZE = 64.0f;   // base units: the size of a lord's face in lord.xml
constexpr f32 LORD_IMAGE_RESOLUTION = 2.0f;   // lords are drawn bigger than their size (phones, the map): render sharper

using DragEventType = chilli::ui::DragEvent::Type;

uilordselect::uilordselect()
{
    disableDrag();
    disableDrop();
}

bool uilordselect::init()
{
    return Widget::init();
}

void uilordselect::updateStatus(character& c)
{
}

std::string uilordselect::faceName(character& c)
{
    if ( Character_IsDead(c) )
        return "f_dead!";

    auto userdata = static_cast<char_data_t*>(getUserData());

    // a lord with a shield (an image of the same name, built from the shield parts) is shown as that shield,
    // scaled to the size of a face
    if ( userdata != nullptr && !userdata->symbol.empty() ) {
        auto shield = StringExtensions::toLower(userdata->symbol);
        if ( imagebuilder::find(shield) != nullptr && SpriteFrameCache::getInstance()->getSpriteFrameByName("shield") != nullptr )
            return shield;
    }

#if defined(_DDR_)
    return GetCharacterFace(c);
#else
    return userdata->face;
#endif
}

void uilordselect::renderLordImage( const std::string& imageId, const std::string& face,
                                    const std::vector<std::string>& show, f32 offsetY )
{
    std::string key = imageId + "|" + face;
    for ( auto& part : show )
        key += "|" + part;
    if ( lordImage != nullptr && key == lordImageKey )
        return;

    auto def = imagebuilder::find(imageId);
    if ( def == nullptr )
        return;

    auto image = *def;

    // an image with circles behind the face is centred on them, so the face is moved inside it; otherwise the
    // picture itself is moved
    const bool circles = image.find("selected") != nullptr;
    const f32 faceShift = circles ? -offsetY / RES(1) : 0;      // base units, y is down in the image

    for ( auto& e : image.elements ) {
        if ( e.id == "face" ) {
            e.name = face;
            e.y += faceShift;
            // a face that is an image (a shield) is scaled to the size of the face (64)
            if ( auto art = imagebuilder::find(face) ) {
                if ( art->width > 0 )
                    e.scale = LORD_FACE_SIZE / art->width;
            }
        } else if ( e.id == "battle" || e.id == "night" || e.id == "dawn" ) {
            e.y += faceShift;
        }
        if ( e.id == "selected" || e.id == "location" || e.id == "drop" || e.id == "battle" || e.id == "night" || e.id == "dawn" )
            e.enabled = std::find(show.begin(), show.end(), e.id) != show.end();
    }

    if ( lordImage == nullptr ) {
        lordImage = Node::create();
        lordImage->setName("face");
        addChild(lordImage);

        // the middle of the picture sits in the middle of the widget (for a face alone, that is the face)
        lordImage->setAnchorPoint(Vec2( 0.5f, 0.5f ));
        auto size = getContentSize();
        lordImage->setPosition(Vec2( size.width / 2, size.height / 2 + ( circles ? 0 : offsetY ) ));

        if ( auto e = image.find("face") ) {
            lordFaceX = RES(e->x);
            lordFaceBottom = RES(image.height - ( e->y + LORD_FACE_SIZE ));
        }
    }

    imagebuilder::setupPicture(lordImage, image, LORD_IMAGE_RESOLUTION);
    lordImageKey = key;
}

// the name is a label on the picture, over the bottom of the face. A single lord's name can wrap to three lines,
// bottom aligned; a grouped lord's short name is centred on the bottom edge of the face.
void uilordselect::setLordTitle( const std::string& text, bool single )
{
    if ( lordImage == nullptr )
        return;

    if ( lordTitle != nullptr ) {
        lordTitle->setString(text);
        return;
    }

    lordTitle = Label::createWithTTF( uihelper::font_config_small, text );
    lordTitle->setName("title");
    lordTitle->setTextColor(Color4B(_clrWhite));
    lordTitle->enableOutline(Color4B(_clrBlack),RES(1));
    lordTitle->getFontAtlas()->setAntiAliasTexParameters();
    lordTitle->setHorizontalAlignment(TextHAlignment::CENTER);
    lordTitle->setVerticalAlignment(TextVAlignment::BOTTOM);

    if ( single ) {
        lordTitle->setLineSpacing(CONTENT_SCALE(0));
        lordTitle->setHeight(RES(FONT_SIZE_SMALL)*3); // three lines in case of very long names
        lordTitle->setWidth(SELECT_ELEMENT_WIDTH-CONTENT_SCALE(4)); // padding of 4 characters for very long names
        lordTitle->setAnchorPoint(uihelper::AnchorBottomCenter);
    } else {
        lordTitle->setAnchorPoint(uihelper::AnchorCenter);
    }
    lordTitle->setPosition(Vec2( lordFaceX, lordFaceBottom ));
    lordImage->addChild(lordTitle);
}

void uilordselect::refreshStatus()
{
}

Vec2 uilordselect::getCenter() const
{
    auto rect = getBoundingBox();
    return Vec2( rect.getMidX(), rect.getMidY() );
}

bool uilordselect::onTouchBegan(Touch *touch, Event* event )
{
   if ( Widget::onTouchBegan(touch, event) ) {
        mouse_down_pos = touch->getLocation();
        mouse_last_position = mouse_down_pos;
        
        uidragevent    dev(this,touch->getLocation(), DragEventType::select);
        dev.lastposition = mouse_down_pos;
        dev.time = utils::getTimeInMilliseconds() ;
        OnSelectDrag(&dev);
        
        return true;
    }
    
    return false;
}

void uilordselect::onTouchMoved(Touch *touch, Event* event )
{
    Widget::onTouchMoved(touch, event);
    
    if ( isDragEnabled() ) {
        Vec2 delta = touch->getLocation() - mouse_down_pos;

        if ( isDragging() ) {
            uidragevent    dev(this,touch->getLocation(),DragEventType::drag);
            dev.lastposition = mouse_last_position;
            dev.time = utils::getTimeInMilliseconds() ;
            OnDrag(&dev);
            setHighlighted(false);
            
        }else if ( ABS(delta.x) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT
                || ABS(delta.y) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
            uidragevent    dev(this,touch->getLocation(),DragEventType::start);
            dev.lastposition = mouse_down_pos;
            dev.time = utils::getTimeInMilliseconds() ;
            OnStartDrag(&dev);
            setHighlighted(false);
        }
        
        event->stopPropagation();
        
    }

    mouse_last_position = touch->getLocation();
    
    
}

void uilordselect::onTouchEnded(Touch *touch, Event* event )
{
    Widget::onTouchEnded(touch, event);
    
    if ( isDragging() ) {
        uidragevent    dev(this,touch->getLocation(),DragEventType::stop);
        dev.time = utils::getTimeInMilliseconds() ;
        OnStopDrag(&dev);
        event->stopPropagation();
        _highlight=false;
    }

    mouse_last_position = touch->getLocation();
}

void uilordselect::onTouchCancelled(Touch *touch, Event* event)
{
    Widget::onTouchCancelled(touch, event);
}

void uilordselect::OnStartDrag(uidragevent *event)
{
    DragMoveElement::OnStartDrag(event);
    refreshStatus();
}

void uilordselect::OnStopDrag(uidragevent* event)
{
    DragMoveElement::OnStopDrag(event);
    
    char_data_t* userdata = static_cast<char_data_t*>( getUserData() );
    if ( userdata != nullptr ) {
        userdata->select_loc.x = getPosition().x;
        userdata->select_loc.y = getPosition().y;
    }
}

void uilordselect::OnDropStart()
{
    DropTarget::OnDropStart();
    refreshStatus();
}

void uilordselect::OnDropStop()
{
    DropTarget::OnDropStop();
    refreshStatus();
}

f32 uilordselect::getDistanceFromCentre( Vec2 pos ) const
{
    auto center = getWorldPosition(); //getCenter();

    // calc the distance from the center
    f32 dx = ABS(center.x - pos.x);
    f32 dy = ABS(center.y - pos.y);
    f32 distance = hypot(dx,dy);

    return distance;
}

UIMOUSEOVER uilordselect::MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const
{
    if ( hint == MOUSE_OVER_HINT_DROP ) {

        f32 scale = getScale();
        
        f32 distance = getDistanceFromCentre(pos);
        
        if ( distance < (DISTANCE_SHIELD * scale) )
            return MOUSE_OVER_FACE ; // face
        
        if ( distance < (DISTANCE_INNER * scale) )
            return MOUSE_OVER_INNER ; // inner
        
    }
    return MOUSE_OVER_NONE ;
}


bool uilordselect::setLord( mxid id )
{
    characterId = id;
    auto userdata = (char_data_t*)TME_GetEntityUserData( characterId );
    setUserData(userdata);
    return true;
}
