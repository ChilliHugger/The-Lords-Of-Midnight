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



namespace tme {


        mxobject::mxobject()
        {
            mxentity::idType = IDT_OBJECT ;
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
#if defined(_ARTEFACTS_)
                WRITE_ENUM(type);
                WRITE_ENUM(power);
#endif
            }else{
                ar >> name;
                ar >> carriedby;
                ar >> description;
                READ_ENUM( kills );
                ar >> usedescription ;
#if defined(_ARTEFACTS_)
                if ( tme::mx->SaveGameVersion() > 10 )
                {
                    READ_ENUM(type);
                    READ_ENUM(power);
                }
#endif
            }
        }
    
        void mxobject::LoadTsv ( const TsvRow& row )
        {
            mxitem::LoadTsv(row);
            Flags().Set(ParseObjectFlags(row.GetString(TsvField::Flags)));

            name = row.GetString(TsvField::Name);
            description = row.GetString(TsvField::Object::Description);
            kills = row.GetThing(TsvField::Object::Kills);
            usedescription = row.GetStringIndex(TsvField::Object::UseDescription);
            carriedby = row.GetItem(TsvField::Object::CarriedBy);
            
#if defined(_ARTEFACTS_)
            type = row.GetObjectType(TsvField::Object::Type);
            power = row.GetObjectPower(TsvField::Object::Power);
#endif
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
        
        void mxobject::Destroy()
        {
            Flags().Reset(of_enabled);
            Flags().Set(of_destroyed);
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
#if defined(_ARTEFACTS_)
            out->type = type;
            out->power = power;
#endif
            return mxitem::FillExportData ( data );
        }


}
// namespace tme
