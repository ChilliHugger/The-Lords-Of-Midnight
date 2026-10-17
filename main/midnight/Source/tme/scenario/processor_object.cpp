#include "../baseinc/tme_internal.h"

namespace tme {

    mxobjectprocessor::mxobjectprocessor()
    {
    }

    mxobjectprocessor::~mxobjectprocessor()
    {
    }

    mxcharacter* mxobjectprocessor::WhoHasObject ( const mxobject* object ) const
    {
        auto carrier = object->CarriedBy();
        if ( carrier != nullptr && carrier->IdType() == IDT_CHARACTER )
            return static_cast<mxcharacter*>(carrier);
        return nullptr;
    }

    mxobject* mxobjectprocessor::FindObjectAtLocation ( mxgridref loc )
    {
        if ( mx->gamemap->HasObject(loc) ) {
            FOR_EACH_OBJECT(object) {
                CONTINUE_IF( object->IsCarried() || object->IsDisabled() || object->IsDestroyed() );
                if ( object->Location() == loc )
                    return object;
            }
        }
        return mx->ObjectById( mx->gamemap->GetAt(loc).object );
    }

    mxobject* mxobjectprocessor::Pickup ( mxcharacter* character )
    {
        auto loc = character->Location();
        auto object = FindObjectAtLocation(loc);
        if ( object == nullptr ) {
#if !defined(_ARTEFACTS_)
            Drop(character);
#endif
            return nullptr;
        }

        // lifted first, as the square can only hold one object and the old one takes its place
        auto previous = character->Carrying();
        RemoveFromMap(object, loc);
        character->CarryObject(object);

        if ( previous != nullptr ) {
            MXTRACE("    %s drops object %s", character->Longname().c_str(), previous->name.c_str());
            PlaceObject(previous, loc);
        }
        return object;
    }

    void mxobjectprocessor::Drop ( mxcharacter* character )
    {
        auto object = character->Carrying();
        character->CarryObject(nullptr);
        PlaceObject(object, character->Location());
    }

    void mxobjectprocessor::PlaceObject ( mxobject* object, mxgridref loc )
    {
        if ( object == nullptr )
            return;

        if ( object->IsArtefact() ) {
            object->Location(loc);
            mx->gamemap->SetObject(loc, true);
        } else {
            mx->gamemap->GetAt(loc).object = object->Id();
        }
    }

    void mxobjectprocessor::RemoveFromMap ( mxobject* object, mxgridref loc )
    {
        if ( !object->IsArtefact() ) {
            mx->gamemap->GetAt(loc).RemoveObject();
            return;
        }

        object->Location(mxgridref(0, 0));
        mx->gamemap->SetObject(loc, AnotherLiesAt(loc));
    }

    bool mxobjectprocessor::AnotherLiesAt ( mxgridref loc ) const
    {
        FOR_EACH_OBJECT(object) {
            if ( !object->IsCarried() && !object->IsDisabled() && !object->IsDestroyed() && object->Location() == loc )
                return true;
        }
        return false;
    }

    void mxobjectprocessor::PlaceObjectsOnMap ( const mxcharacter* character )
    {
        FOR_EACH_OBJECT(object) {
            CONTINUE_IF( !object->IsArtefact() );

            if ( object->IsRandomStart() ) {
                mxgridref loc;
                do {
                    loc.x = mxrandom(0,mx->gamemap->Size().cx-1);
                    loc.y = mxrandom(0,mx->gamemap->Size().cy-1);
                } while ( mx->gamemap->HasObject(loc) || mx->scenario->isLocationImpassable(loc,character) );
                object->Location(loc);
            }
            mx->gamemap->SetObject( object->Location(), true );
        }
    }

    bool mxobjectprocessor::Give ( mxcharacter* from, mxcharacter* to )
    {
        if ( to == nullptr || to->IsCarryingObject() )
            return false;

        auto object = from->Carrying();
        from->CarryObject(nullptr);
        to->CarryObject(object);
        return true;
    }

    void mxobjectprocessor::Exchange ( mxcharacter* first, mxcharacter* second )
    {
        auto one = first->Carrying();
        auto other = second->Carrying();
        first->CarryObject(nullptr);
        second->CarryObject(nullptr);
        first->CarryObject(other);
        second->CarryObject(one);
    }

    void mxobjectprocessor::Destroy ( mxobject* object )
    {
        auto holder = WhoHasObject(object);
        if ( holder != nullptr )
            holder->CarryObject(nullptr);
        else if ( object->IsArtefact() )
            RemoveFromMap(object, object->Location());
        object->Destroy();
    }

}
