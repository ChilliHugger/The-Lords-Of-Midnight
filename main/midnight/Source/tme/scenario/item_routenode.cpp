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
            branches.clear();
        }

        mxroutenode::~mxroutenode()
        {
            ClearNodes();
        }

        void mxroutenode::ClearNodes()
        {
            for ( auto branch : branches )
                delete branch;
            branches.clear();
        }

        void mxroutenode::Serialize ( archive& ar )
        {
        u8 count = 2;
        
            mxitem::Serialize(ar);
            if ( ar.IsStoring() ) {
                ar << (u8)branches.size();
                for ( auto branch : branches )
                    ar << branch;
            }else{
                if ( mx->SaveGameVersion() > 16 ) {
                    ar >> count;
                }
                ClearNodes();
                for (int ii=0; ii<count; ii++) {
                    auto branch = mx->entityfactory->CreateRouteBranch();
                    ar >> branch;
                    branches.push_back( branch );
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

        archive& operator<<(archive& ar, mxroutebranch* branch)
        {
            branch->Serialize(ar);
            return ar;
        }

        archive& operator>>(archive& ar, mxroutebranch*& branch)
        {
            branch->Serialize(ar);
            return ar;
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
        
        c_node mxroutenode::Nodes() const
        {
            c_node nodes;
            for ( auto& branch : branches )
                nodes.push_back(branch->Node());
            return nodes;
        }

        mxroutebranch* mxroutenode::AddNode( mxroutenode* node )
        {
            if ( node == nullptr )
                return nullptr;

            auto branch = mx->entityfactory->CreateRouteBranch();
            branch->Node(node);
            branches.push_back(branch);
            return branch;
        }

        void mxroutebranch::Serialize ( archive& ar )
        {
            if ( ar.IsStoring() )
                ar << node;
            else
                ar >> node;
        }

        mxroutenode* mxroutenode::PickNode()
        {
            auto index = mxrandom((u32)branches.size()-1);
            return branches[index]->Node();
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
