/*
 * FILE:    discoverymap.cpp
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

using namespace chilli::lib;

namespace tme {
    
    
    mxdiscoverymap::mxdiscoverymap()
    {
        m_size = size(0,0);
        m_data = NULL ;
    }
    
    mxdiscoverymap::~mxdiscoverymap()
    {
        SAFEFREE ( m_data );
    }
    
    
    mxdiscoverymap::mxdiscoverymap( u32 x, u32 y)
    {
        m_data = NULL ;
        Create ( size(x,y) );
    }
    
    
    /*
     * Function name    : map::Create
     *
     * Return type        : bool
     *
     * Arguments        : CSize dimensions
     *
     * Description        :
     *
     */
    
    bool mxdiscoverymap::Create ( size dimensions )
    {
        m_size = dimensions ;
        SAFEFREE ( m_data );
        int bytes = m_size.cx*m_size.cy*sizeof(flags32);
        if ( (m_data = (flags32*)malloc( bytes ) ) == NULL ) {
            return FALSE;
        }
        memset ( m_data, 0x00, bytes );
        
        ResetVisibleArea();
        
        return TRUE ;
    }
    
    void mxdiscoverymap::ClearVisibleArea()
    {
        ResetVisibleArea();
    }
    
    void mxdiscoverymap::ResetVisibleArea()
    {
        m_top_visible.x=m_size.cx;
        m_top_visible.y=m_size.cy;
        
        m_bottom_visible.x=0;
        m_bottom_visible.y=0;
    }
    
    
    /*
     * Function name    : map::Load
     *
     * Return type        : bool
     *
     * Arguments        : LPCSTR filename
     *
     * Description        : Load the map from an external file
     *
     */
    
    bool mxdiscoverymap::Load ( LPCSTR filename )
    {
        MXTRACE("Loading Discovery Map '%s'", filename);

        chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeRead );
        if ( !pFile->IsOpen() ) {
            if ( pFile ) delete pFile;
            return FALSE;
        }

        archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

        u32 magicno;
        u32 versionno;
        c_str header;

        ar >> magicno;
        MXTRACE("MagicNo=%x", (unsigned int)magicno);

        // check for valid header and possible byte swap version
        if ( magicno == chilli::lib::u32Swap(TME_MAGIC_NO) ) {
            // turn on byte swapping
            MXTRACE( "ByteSwapping ON");
            ar.m_nMode |= archive::bByteSwap ;
        }else{
            if ( magicno != TME_MAGIC_NO ) {
                MXTRACE("Invalid DISCOVERYMAP MagicNo");
                return FALSE;
            }
        }

        ar >> versionno;
        MXTRACE( "Version=%d", (int)versionno);
        if ( versionno != DISCOVERYVERSION ) {
            MXTRACE("Invalid DISCOVERYMAP Version");
            return FALSE;
        }

        ar >> header;
        MXTRACE("Header='%s'", (LPSTR)header);

        if (c_stricmp(header,DISCOVERYHEADER) != 0 ) {
            MXTRACE("Invalid DISCOVERYMAP Header");
            return FALSE;
        }

        Serialize(ar);

        ar.Close();

        SAFEDELETE ( pFile );

        CalculateVisibleArea();
        
        return TRUE ;
    }
    
    bool mxdiscoverymap::Save ( LPCSTR filename )
    {
        chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
        if ( pFile == NULL || !pFile->IsOpen() ) {
            if ( pFile ) delete pFile;
            //COMPLAIN( "Cannot Save data file %s", filename );
            return MX_FAILED;
        }
        
        archive ar (pFile, archive::store | archive::bNoFlushOnDelete );
        
        ar << TME_MAGIC_NO ;
        ar << DISCOVERYVERSION ;
        ar << (char*)DISCOVERYHEADER ;
        
        Serialize(ar);
        
        ar.Close();
        
        SAFEDELETE ( pFile );
        
        return TRUE;
        
        
    }
    
    
    
    /*
     * Function name    : map::Serialize
     *
     * Return type        : void
     *
     * Arguments        : archive& ar
     *
     * Description        : Serialize the map into the save/load game file
     *
     */
    void mxdiscoverymap::Serialize ( archive& ar )
    {
        MXTRACE( "mxdiscoverymap::Serialize");
        
        if ( ar.IsStoring() ) {
            ar << m_size ;
            
        }else{
            ar >> m_size ;
            Create ( m_size );
        }
        
        MXTRACE("MapSize:: Width = %d Height=%d)", (int)m_size.cx, (int)m_size.cy);
        
        if ( ar.IsStoring() ) {
            for ( int ii=0; ii<m_size.cx*m_size.cy; ii++ ) {
                ar << m_data[ii] ;
            }
        }else{
            for ( int ii=0; ii<m_size.cx*m_size.cy; ii++ ) {
                ar >> m_data[ii] ;
                
            }
            CalculateVisibleArea();
        }
        
        MXTRACE( "mxdiscoverymap::Serialize::OK");
    }
    
    
    
    bool mxdiscoverymap::IsLocOnMap ( mxgridref mxloc ) const
    {
        if ( mxloc.x < 0 || mxloc.x >= m_size.cx || mxloc.y < 0 || mxloc.y >= m_size.cy ) {
            return FALSE;
        }
        return TRUE;
    }

    
    /*
     * Function name    : map::GetAt
     *
     * Return type        : flags8_t
     *
     * Arguments        : const gridref& loc
     *
     * Description        :
     *
     */
    
    flags32& mxdiscoverymap::GetAt ( const mxgridref& mxloc )
    {
        static flags32 blank;
        
        // out of bounds map reference
        if ( !IsLocOnMap(mxloc) ) {
            return blank;
        }
        
        int offset = ((mxloc.y)*m_size.cx) + (mxloc.x) ;
        
        return m_data[offset];
    }
    
    
    bool mxdiscoverymap::IsLocationVisible( mxgridref l )
    {
        return GetAt( l ).Is(lf_seen);
    }
    
#if defined(_DDR_)
    bool mxdiscoverymap::IsTunnelVisible( mxgridref l )
    {
        return GetAt( l ).Is( lf_tunnel_looked_at);
    }
    
    void mxdiscoverymap::SetTunnelVisible( mxgridref l, bool visible )
    {
        flags32& mapsqr = GetAt( l );
        if ( visible ) {
            mapsqr.Set(lf_tunnel_looked_at);
            CheckVisibleArea(l);
        } else
            mapsqr.Reset(lf_tunnel_looked_at);
    }
    
    
#endif
    
    void mxdiscoverymap::SetLocationVisible( mxgridref l, bool visible )
    {
        flags32& mapsqr = GetAt( l );
        if ( visible ) {
            mapsqr.Set(lf_seen);
            CheckVisibleArea(l);
        } else
            mapsqr.Reset(lf_seen);
    }
    
    void mxdiscoverymap::SetLocationLookedAt( mxgridref l, bool visible )
    {
        flags32& mapsqr = GetAt( l );
        if ( visible ) {
            mapsqr.Set(lf_looked_at);
        } else
            mapsqr.Reset(lf_looked_at);
    }
    
    void mxdiscoverymap::SetLocationVisited( mxgridref l, bool visible )
    {
        flags32& mapsqr = GetAt( l );
        if ( visible ) {
            mapsqr.Set(lf_visited);
        } else
            mapsqr.Reset(lf_visited);
    }
    
    
    void mxdiscoverymap::CheckVisibleArea( mxgridref l )
    {
        if ( l.x <= m_top_visible.x )
            m_top_visible.x = std::max(l.x,0);
        if ( l.x >= m_bottom_visible.x )
            m_bottom_visible.x = std::min(l.x,m_size.cx);
        if ( l.y <= m_top_visible.y )
            m_top_visible.y = std::max(l.y,0);
        if ( l.y >= m_bottom_visible.y )
            m_bottom_visible.y = std::min(l.y,m_size.cy);
    }
    
    void mxdiscoverymap::CalculateVisibleArea()
    {
        mxgridref loc;
        
        m_top_visible.x=m_size.cx;
        m_top_visible.y=m_size.cy;
        
        m_bottom_visible.x=0;
        m_bottom_visible.y=0;
        
        for ( int y=0; y<m_size.cy; y++ ) {
            for ( int x=0; x<m_size.cx; x++ ) {
                loc.x=x;
                loc.y=y;
                if ( IsLocationVisible(loc) ) {
                    CheckVisibleArea(loc);
                }
            }
        }
        
        
    }

    void mxdiscoverymap::TransferFromMap( mxmap* map )
    {
        mxgridref loc;

        for ( int y=0; y<m_size.cy; y++ ) {
            for ( int x=0; x<m_size.cx; x++ ) {
                loc.x=x;
                loc.y=y;
                
                mxloc l = map->GetAt(loc);
                
                
                GetAt(loc).Set( l.flags & (lf_seen|lf_visited|lf_looked_at) );
                
#if defined(_DDR_)
                GetAt(loc).Set( l.flags&(lf_tunnel_looked_at|lf_tunnel_visited) ) ;
#endif
            }
        }
        
        CalculateVisibleArea();
        
    }
    
    
    
}
// naemspace tme
    
