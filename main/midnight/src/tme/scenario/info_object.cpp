/*
 * FILE:    info_object.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2011 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */


#include "../baseinc/tme_internal.h"
#include "../utils/savegamemapping.h"

namespace tme {


        mxobject::mxobject()
        {
            mxentity::type = IDT_OBJECT ;
        }

        mxobject::~mxobject()
        {
        }

        void mxobject::Serialize ( archive& ar )
        {
            mxitem::Serialize ( ar );

            if ( ar.IsStoring() ) {
                ar << name;
                ar << carriedby;
                ar << description;
                WRITE_ENUM( kills );
                ar << usedescription ;
                WRITE_ENUM( thing );
            }else{
                ar >> name;
                ar >> carriedby;
                ar >> description;
                READ_ENUM( kills );
                ar >> usedescription ;
                if ( tme::mx->SaveGameVersion()>15)
                    READ_ENUM( thing );
                else
                    utils::UpdateObjectThing(this);
            }
        }
    
        bool mxobject::CanDestroy ( mxobject* obj ) const
        {
            return (u32)kills == (u32)obj->Id() ;
        }

        archive& operator<<(archive& ar, mxobject* ptr )
        {
            return ar << (u32) mxentity::SafeId(ptr);
        }

        archive& operator>>( archive& ar, mxobject*& ptr )
        {
        int temp;
            ar >> temp ; ptr = mx->ObjectById(temp);
            return ar ;
        }

        bool mxobject::IsCarried() const
        {
            return carriedby != NULL ;
        }
    
        MXRESULT mxobject::FillExportData ( info_t* data )
        {
            defaultexport::object_t* out = (defaultexport::object_t*)data;
            VALIDATE_INFO_BLOCK(out,INFO_OBJECT,defaultexport::object_t);
            out->kills = kills;
            out->name = std::string(name);
            out->description = std::string(description);
            out->usedescription = MAKE_ID(IDT_STRING,usedescription);
            out->carriedby = SafeIdt(carriedby);
            return mxitem::FillExportData ( data );
        }


}
// namespace tme
