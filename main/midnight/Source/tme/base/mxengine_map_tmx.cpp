/*
 *  mxengine_map_tmx.cpp
 *  tme
 *
 *  Created by Chris Wild on 02/02/2011.
 *  Copyright 2011 Chilli Hugger. All rights reserved.
 *
 */

#include "../baseinc/tme_internal.h"
#include "../TMEMapBuilder.h"
#include "2d/TMXXMLParser.h"
#include "axmol.h"
#include <cstdlib>
#include <functional>
#include <unordered_map>

USING_NS_AX;
USING_NS_TME ;

class TemporaryRouteNode : public mxroutenode {
    public:
        void init(mxid id, std::string symbol) {
            this->id = id;
            this->symbol = symbol;
        }
};

class TemporaryPlace : public mxplace {
    public:
        void init(mxid id, std::string symbol) {
            this->id = id;
            this->symbol = symbol;
        }
};



MXRESULT mxengine::LoadMapTmx ( const std::string& tmxFilename )
{
    if ( !chilli::os::filemanager::Exists(tmxFilename) )
        return MX_FAILED;

    MXTRACE( "Building Map from TMX '%s'", tmxFilename.c_str());

    TMEMapBuilder builder;
    gamemap = builder.Build(tmxFilename);

    if ( gamemap == nullptr )
        return MX_FAILED;

    return MX_OK ;
}

namespace {

    // TMX "object" type properties (regiment TARGET, routenode LEFT/RIGHT)
    // store the referenced object's map-wide numeric id, not its name - so
    // build an id -> name lookup across every object group once, up front
    std::unordered_map<int, std::string> BuildObjectIdIndex ( TMXMapInfo* mapInfo )
    {
        std::unordered_map<int, std::string> index;

        for ( const auto& group : mapInfo->getObjectGroups() ) {
            for ( const auto& value : group->getObjects() ) {
                CONTINUE_IF (value.getType() != Value::Type::MAP );

                const ValueMap& dict = value.asValueMap();

                auto itId   = dict.find("id");
                auto itName = dict.find("name");
                if ( itId == dict.end() || itName == dict.end() )
                    continue;

                index[ itId->second.asInt() ] = itName->second.asString();
            }
        }

        return index;
    }

    // axmol's TMX object parsing (TMXXMLParser.cpp) converts each object's
    // raw TMX pixel x/y/height (top-left anchored, Y-down) into Cocos2D
    // render space (Y-up, pixels-to-points) for placing sprites:
    //
    //   p.x = (x + offset.x) / scale
    //   p.y = (mapHeight*tileHeight - y - offset.y - height) / scale
    //
    // so it has to be reversed here to recover the raw tile-grid
    // coordinates that match the TSV/binary database.
    mxgridref ObjectGridLocation ( const ValueMap& dict, const TMXObjectGroup* group, float mapHeightPixels, const Vec2& tileSize, float scale )
    {
        Vec2 offset = group->getPositionOffset();

        float xPixels = dict.at("x").asFloat() * scale - offset.x;
        float yPixels = mapHeightPixels - ( dict.at("y").asFloat() * scale ) - offset.y - ( dict.at("height").asFloat() * scale );

        return mxgridref ( (u32)(xPixels / tileSize.width), (u32)(yPixels / tileSize.height) );
    }

    using EntityLookupFn = std::function<mxentity*(const std::string&, id_type_t)>;

    // resolves a TMX "object" type property's stored map-wide object id
    // back to the entity it names, via the id -> name index
    mxentity* ResolveObjectProperty ( const EntityLookupFn& entityByName, const ValueMap& dict, const char* propertyName, const std::unordered_map<int, std::string>& objectIdToName, id_type_t idType = IDT_NONE )
    {
        auto itProperty = dict.find(propertyName);
        if ( itProperty == dict.end() )
            return nullptr;

        int objectId = atoi( itProperty->second.asString().c_str() );

        auto itName = objectIdToName.find(objectId);
        if ( itName == objectIdToName.end() )
            return nullptr;

        return entityByName(itName->second, idType);
    }

}

MXRESULT mxengine::ApplyMapEntitiesFromTmx ( const std::string& tmxFilename )
{
    std::unique_ptr<TMXMapInfo> mapInfo(new TMXMapInfo());
    if ( !mapInfo->initWithTMXFile(tmxFilename) ) {
        return MX_FAILED;
    }

    Vec2 mapSize  = mapInfo->getMapSize();
    Vec2 tileSize = mapInfo->getTileSize();
    float mapHeightPixels = mapSize.height * tileSize.height;
    float scale = AX_CONTENT_SCALE_FACTOR();

    auto objectIdToName = BuildObjectIdIndex(mapInfo.get());

    EntityLookupFn entityByName = [this] ( const std::string& name, id_type_t type ) {
        return EntityByName(name, type);
    };

    // groups whose object x/y directly give the entity's map location,
    // exactly matching the TSV/binary database
    static const struct { const char* group; id_type_t idType; } locationGroups[] = {
        { "routenodes",  IDT_ROUTENODE },
        { "waypoints",   IDT_PLACE },
        { "characters",  IDT_CHARACTER },
        { "strongholds", IDT_STRONGHOLD },
        { "regiments",   IDT_REGIMENT },
    };

    for ( const auto& group : mapInfo->getObjectGroups() ) {

        const std::string groupName( group->getGroupName() );

        for ( const auto& locationGroup : locationGroups ) {

            CONTINUE_IF( c_stricmp( groupName.c_str(), locationGroup.group ) != 0 );

            for ( const auto& value : group->getObjects() ) {
                CONTINUE_IF (value.getType() != Value::Type::MAP );

                const ValueMap& dict = value.asValueMap();

                auto itName = dict.find("name");
                CONTINUE_IF(itName == dict.end());

                std::string name = itName->second.asString();

                auto entity = EntityByName(name, locationGroup.idType);
                if ( entity == nullptr ) {
                    // if routenode or waypoint we can add one
                    if (locationGroup.idType == IDT_ROUTENODE) {
                        sv_routenodes++;
                        auto routenode = static_cast<TemporaryRouteNode*>(entityfactory->Create(IDT_ROUTENODE));
                        routenode->init(MAKE_ID(IDT_ROUTENODE, sv_routenodes), name);
                        objRoutenodes.Add(routenode);
                        entity = routenode;
                    } else if (locationGroup.idType == IDT_PLACE) {
                        sv_places++;
                        auto place = static_cast<TemporaryPlace*>(entityfactory->Create(IDT_PLACE));
                        place->init(MAKE_ID(IDT_PLACE, sv_places), name);
                        objPlaces.Add(place);
                        entity = place;
                    } else {
                        MXTRACE( "TMX object '%s' not found in database", name.c_str());
                        continue;
                    }
                }

                ((mxitem*)entity)->Location( ObjectGridLocation(dict, group, mapHeightPixels, tileSize, scale) );
            }

            break;
        }
    }

    // regiments: TARGET is a reference to another object's map-wide id
    for ( const auto& group : mapInfo->getObjectGroups() ) {

        CONTINUE_IF(c_stricmp( std::string(group->getGroupName()).c_str(), "regiments" ) != 0);

        for ( const auto& value : group->getObjects() ) {
            CONTINUE_IF (value.getType() != Value::Type::MAP );

            const ValueMap& dict = value.asValueMap();

            auto itName = dict.find("name");
            CONTINUE_IF(itName == dict.end());

            auto regiment = static_cast<mxregiment*>(EntityByName(itName->second.asString(), IDT_REGIMENT));
            CONTINUE_IF_NULL(regiment);

            auto target = ResolveObjectProperty(entityByName, dict, "TARGET", objectIdToName);
            if ( target == nullptr ) {
                MXTRACE( "TMX regiment '%s' TARGET not resolved", itName->second.asString().c_str());
                continue;
            }

            regiment->TargetId( mxentity::SafeIdt(target) );
        }
    }

    // routenodes: LEFT/RIGHT are references to other routenode objects' ids
    for ( const auto& group : mapInfo->getObjectGroups() ) {

        CONTINUE_IF( c_stricmp( std::string(group->getGroupName()).c_str(), "routenodes" ) != 0 );

        for ( const auto& value : group->getObjects() ) {
            CONTINUE_IF (value.getType() != Value::Type::MAP );
            const ValueMap& dict = value.asValueMap();

            auto itName = dict.find("name");
            CONTINUE_IF(itName == dict.end());

            auto routenode = static_cast<mxroutenode*>(EntityByName(itName->second.asString(), IDT_ROUTENODE));
            CONTINUE_IF_NULL(routenode);

#if defined(_LOM_)
            auto left  = ResolveObjectProperty(entityByName, dict, "LEFT",  objectIdToName, IDT_ROUTENODE);
            auto right = ResolveObjectProperty(entityByName, dict, "RIGHT", objectIdToName, IDT_ROUTENODE);

            if ( left == nullptr || right == nullptr ) {
                MXTRACE( "TMX routenode '%s' LEFT/RIGHT not resolved", itName->second.asString().c_str());
                continue;
            }

            routenode->Left ( static_cast<mxroutenode*>(left) );
            routenode->Right( static_cast<mxroutenode*>(right) );
#endif

#if defined(_CITADEL_)
            auto left  = ResolveObjectProperty(entityByName, dict, "BRANCH_0",  objectIdToName, IDT_ROUTENODE);
            auto middle = ResolveObjectProperty(entityByName, dict, "BRANCH_1", objectIdToName, IDT_ROUTENODE);
            auto right = ResolveObjectProperty(entityByName, dict, "BRANCH_2", objectIdToName, IDT_ROUTENODE);

            if ( left == nullptr || middle == nullptr || right) {
                MXTRACE( "TMX routenode '%s' branches not resolved", itName->second.asString().c_str());
                continue;
            }

            routenode->Left ( static_cast<mxroutenode*>(left) );
            routenode->Middle( static_cast<mxroutenode*>(middle) );
            routenode->Right( static_cast<mxroutenode*>(right) );
#endif

        }
    }

    return MX_OK;
}
