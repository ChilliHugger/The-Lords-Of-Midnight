/*
 * FILE:    item_regiment.cpp
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

        mxregiment::mxregiment() :
            targetid(0),
            total(0),
            orders(OD_NONE),
            lost(0)
        {
            mxentity::type = IDT_REGIMENT ;
        }

        mxregiment::~mxregiment()
        {
        }

        void mxregiment::Serialize ( archive& ar )
        {
            mxitem::Serialize(ar);
            if ( ar.IsStoring() ) {
                WRITE_ENUM ( race );
                WRITE_ENUM ( type );
                ar << total;
                ar << targetid;
                WRITE_ENUM ( orders );
                ar << success;
                ar << loyalty;
                ar << killed ;
                ar << lastlocation ;
                ar << delay;
            }else{
                READ_ENUM ( race );
                READ_ENUM ( type );
                ar >> total;
                ar >> targetid;
                READ_ENUM ( orders );
                ar >> success;
                ar >> loyalty;
                ar >> killed;
                lost=0;
                lastlocation = Location();
                delay=0;
              
                if ( tme::mx->SaveGameVersion() > 3 )
                    ar >> lastlocation ;

                if ( tme::mx->SaveGameVersion() > 6 )
                    ar >> delay ;
            }
        }

        s32 mxregiment::BattleSuccess ( const mxlocinfo& locinfo )
        {
            return success + mx->battle->BaseDoomdarkSuccess(Race(),Type(),locinfo);
        }

        MXRESULT mxregiment::FillExportData ( info_t* data )
        {
            // check if we are being asked to export the regiment as
            // army
            if ( (u32)ID_TYPE(data->id) == (u32)INFO_ARMY ) {
                defaultexport::army_t* out = (defaultexport::army_t*)data;
                out->parent = SafeIdt(this);
                out->race = Race();
                out->type = AT_REGIMENT ;
                out->total = Total() ;
                out->energy = 0;
                out->lost = 0;
                out->killed = Killed();
                return MX_OK ;
            }

            defaultexport::regiment_t* out = (defaultexport::regiment_t*)data;

            VALIDATE_INFO_BLOCK(out,INFO_REGIMENT,defaultexport::regiment_t);

            out->race = Race();
            out->type = Type();
            out->total = Total();
            out->target = TargetId();
            out->orders = Orders();
            out->success = success;
            out->killed = Killed();
            out->loyalty = SafeIdt(loyalty);
            out->lastlocation = lastlocation ;
            return mxitem::FillExportData ( data );
        }
}
// namespace tme
