/*
 * FILE:    item_routenode.cpp
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

        mxroutenode::mxroutenode()
        {
            mxentity::idType = IDT_ROUTENODE ;
            nodes.clear();
        }

        mxroutenode::~mxroutenode()
        {
        }

        void mxroutenode::Serialize ( archive& ar )
        {
        mxroutenode* node = nullptr;
        u8 count = 2;
        
            mxitem::Serialize(ar);
            if ( ar.IsStoring() ) {
                count = nodes.size();
                ar << count;
                for (int ii=0; ii<count; ii++)
                    ar << nodes[ii] ;
            }else{
                if ( mx->SaveGameVersion() > 16 ) {
                    ar >> count;
                }
                ClearNodes();
                for (int ii=0; ii<count; ii++) {
                    ar >> node ;
                    AddNode(node);
                }
            }
        }

        void mxroutenode::LoadTsv ( const TsvRow& row )
        {
            mxitem::LoadTsv(row);

            auto list = row.GetSymbolList(TsvField::RouteNode::RouteNodes, ',');
            ClearNodes();
            for ( auto& item : list ) {
                auto routenode = ResolveRouteNode(row.Symbols(), item);
                if (routenode!=nullptr) {
                    AddNode(routenode);
                }
            }
        }

        archive& operator<<(archive& ar, mxroutenode* node)
        {
            return ar << ((u32)mxentity::SafeId(node));
        }

        archive& operator>>( archive& ar, mxroutenode*& node)
        {
        int temp;
            ar >> temp ; node = (mxroutenode*)mx->RouteNodeById(temp);
            return ar ;
        }
        
        mxroutenode* mxroutenode::PickNode()
        {
            auto index = mxrandom((u32)nodes.size()-1);
            return nodes[index];
        }
        

        MXRESULT mxroutenode::FillExportData ( info_t* data )
        {
            defaultexport::routenode_t* out = (defaultexport::routenode_t*)data;

            VALIDATE_INFO_BLOCK(out,INFO_ROUTENODE,defaultexport::routenode_t);
            
//            for ( u32 ii=0; ii<NUMELE(paths); ii++ )
//                out->paths[ii] = SafeIdt(paths[ii]);

            return mxitem::FillExportData ( data );
        }


}
// namespace tme
