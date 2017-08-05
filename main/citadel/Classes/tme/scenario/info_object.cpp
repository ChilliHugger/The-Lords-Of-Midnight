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
                
#if defined(_DDR_)
                WRITE_ENUM(type);
                WRITE_ENUM(power);
#endif
                
			}else{
				ar >> name;
				ar >> carriedby;
				ar >> description;
				READ_ENUM( kills );
				ar >> usedescription ;

#if defined(_DDR_)
                if ( tme::mx->SaveGameVersion() > 10 )
                {
                    READ_ENUM(type);
                    READ_ENUM(power);
                }
#endif
			}
		}
    
		BOOL mxobject::CanDestroy ( mxobject* obj ) const
		{
			return (u32)kills == (u32)obj->Id() ;
		}

#if defined(_DDR_)
        BOOL mxobject::IsCarried() const
        {
            return carriedby != NULL ;
        }
    
        BOOL mxobject::IsSpecial() const
        {
            if (IsSymbol("OB_CROWN_VARENAND"))
                return true;
            if (IsSymbol("OB_CROWN_CARUDRIUM"))
                return true;
            if (IsSymbol("OB_SPELL_THIGRORN"))
                return true;
            if (IsSymbol("OB_RUNES_FINORN"))
                return true;
            if (IsSymbol("OB_CROWN_IMIRIEL"))
                return true;
            return FALSE;
        }
#endif
        archive& operator<<(archive& ar, mxobject* ptr )
		{
			return ar << (DWORD) ptr->SafeId();
		}

		archive& operator>>( archive& ar, mxobject*& ptr )
		{
		int temp;
			ar >> temp ; ptr = mx->ObjectById(temp);
			return ar ;
		}

		MXRESULT mxobject::FillExportData ( info_t* data )
		{
			defaultexport::object_t* out = (defaultexport::object_t*)data;
			VALIDATE_INFO_BLOCK(out,INFO_OBJECT,defaultexport::object_t);
			out->type = (mxthing_t)Id() ;
			out->kills = kills;
			out->name = name;
			out->description = description;
			out->usedescription = MAKE_ID(IDT_STRING,usedescription);
			return mxitem::FillExportData ( data );
		}


}
// namespace tme
