/*
 * FILE:    map.cpp
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

static mxloc blank;

mxmap::mxmap()
{
	m_size = size(0,0);
	m_data = NULL ;
    m_discoverymap = NULL ;
}

mxmap::~mxmap()
{
	SAFEFREE ( m_data );
}


mxmap::mxmap( u32 x, u32 y)
{
	m_data = NULL ;
    m_discoverymap = NULL ;

	Create ( size(x,y) );
}


/*
 * Function name	: map::Create
 * 
 * Return type		: BOOL
 * 
 * Arguments		: CSize dimensions
 * 
 * Description	    : 
 * 
 */

BOOL mxmap::Create ( size dimensions )
{
	m_size = dimensions ;
	SAFEFREE ( m_data );
	int bytes = m_size.cx*m_size.cy*sizeof(mxloc);
	if ( (m_data = (mxloc*)malloc( bytes ) ) == NULL ) {
		return FALSE;
	}
	memset ( m_data, 0x00, bytes );

    ResetVisibleRange();
    
//	blank.terrain=TN_CITADEL;
	
	return TRUE ;
}

void mxmap::ClearVisible()
{
    ResetVisibleRange();
	for ( int ii=0; ii<m_size.cx*m_size.cy; ii++ ) 
		m_data[ii].flags&=~lf_special;    
}
    
void mxmap::ResetVisibleRange()
{
    m_top_visible.x=m_size.cx;
    m_top_visible.y=m_size.cy;

    m_bottom_visible.x=0;
    m_bottom_visible.y=0;
}
    

/*
 * Function name	: map::Load
 * 
 * Return type		: BOOL
 * 
 * Arguments		: LPCSTR filename
 * 
 * Description	    : Load the map from an external file
 * 
 */

BOOL mxmap::Load ( LPCSTR filename )
{
	
MXTRACE( "Loading Map '%s'", filename);

	chilli::os::file* pFile = new chilli::os::file ( filename, chilli::os::file::modeRead );
	if ( !pFile->IsOpen() ) {
		if ( pFile ) delete pFile;
		return FALSE;
	}

	archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

u32 magicno;
u32 versionno;
string header;

	ar >> magicno;
MXTRACE( "MagicNo=%x", (unsigned int)magicno);

	// check for valid header and possible byte swap version
	if ( magicno == chilli::lib::u32Swap(TME_MAGIC_NO) ) {
		// turn on byte swapping
MXTRACE( "ByteSwapping ON");
		ar.m_nMode |= archive::bByteSwap ;
	}else{
        if ( magicno != TME_MAGIC_NO ) {
            MXTRACE("Invalid MAP MagicNo");
            return FALSE;
        }
	}

	ar >> versionno;
MXTRACE( "Version=%d", (int)versionno);
    if ( versionno != MAPVERSION ) {
        MXTRACE("Invalid MAP VersionNO");
        return FALSE;
    }
	
	ar >> header;
MXTRACE( "Header='%s'", (LPSTR)header);

    if ( chilli::lib::stricmp(header,MAPHEADER) != 0 ) {
        MXTRACE("Invalid MAP Header");
		return FALSE;
    }

	Serialize(ar);

	ar.Close();

	SAFEDELETE ( pFile );
    
    CalculateVisibleArea();

	return TRUE ;
}

/*
 * Function name	: map::Serialize
 * 
 * Return type		: void
 * 
 * Arguments		: archive& ar
 * 
 * Description	    : Serialize the map into the save/load game file
 * 
 */
void mxmap::Serialize ( archive& ar ) 
{
MXTRACE( "mxmap::Serialize");
	
	if ( ar.IsStoring() ) {
		ar << m_size ;
	}else{
		ar >> m_size ;
		Create ( m_size );
	}

MXTRACE( "MapSize:: Width = %d Height=%d)", (int)m_size.cx, (int)m_size.cy);
	
	for ( int ii=0; ii<m_size.cx*m_size.cy; ii++ ) {
		m_data[ii].Serialize(ar);
    
        
//#ifdef _DDR_
//        if ( m_data[ii].HasTunnel() )
//            m_data[ii].flags |= lf_tunnel_seen ;
//#endif
    
    }
	
    if ( ar.IsLoading() )
        CalculateVisibleArea();
    
MXTRACE( "mxmap::Serialize::OK");
}





/*
 * Function name	: map::GetAt
 * 
 * Return type		: loc&
 * 
 * Arguments		: const gridref& loc
 * 
 * Description	    : 
 * 
 */

mxloc& mxmap::GetAt ( const mxgridref& mxloc )
{
	// out of bounds map reference
	if ( !IsLocOnMap(mxloc) ) {
		return blank;
	}
	
	int offset = ((mxloc.y)*m_size.cx) + (mxloc.x) ;

	return m_data[offset];
}


/*
 * Function name	: map::IsLocationSpecial
 * 
 * Return type		: BOOL
 * 
 * Arguments		: gridref loc
 * 
 * Description	    : 
 * 
 */

BOOL mxmap::IsLocationSpecial( mxgridref l )
{
	mxloc& mapsqr = GetAt( l );
	return mapsqr.IsSpecial();
}
    
BOOL mxmap::IsLocationVisible( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    return mapsqr.IsVisible()  ;
}
    
#if defined(_DDR_)
BOOL mxmap::HasTunnelEntrance( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    return mapsqr.HasTunnelEntrance() ;
}
BOOL mxmap::HasTunnelExit( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    return mapsqr.HasTunnelExit() ;
}
    
    
BOOL mxmap::IsTunnelVisible( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    return mapsqr.IsTunnelVisible() ;
}

BOOL mxmap::IsTunnel( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    return mapsqr.HasTunnel() ;
}
    
BOOL mxmap::IsTunnelPassageway( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    mxterrain_t t = mx->scenario->NormaliseTerrain((mxterrain_t)mapsqr.terrain);
    return mapsqr.HasTunnel() && (t ==TN_PLAINS2 || t==TN_MOUNTAIN2 || t==TN_FOREST2 || t==TN_HILLS ) ;
}

void mxmap::SetTunnelVisible( mxgridref l, BOOL visible )
{
    mxloc& mapsqr = GetAt( l );
    if ( visible ) {
        mapsqr.flags|=lf_tunnel_looked_at;
        CheckVisibleRange(l);
    } else
        mapsqr.flags&=~lf_tunnel_looked_at;
}
    
BOOL mxmap::HasObject( mxgridref l )
{
    mxloc& mapsqr = GetAt( l );
    return mapsqr.HasObject() ;
}
    
void mxmap::SetObject( mxgridref l, BOOL value )
{
    mxloc& mapsqr = GetAt( l );
    if ( value ) {
        mapsqr.flags|=lf_object;
    } else
        mapsqr.flags&=~lf_object;
}

    
    
#endif
    
/*
 * Function name	: map::IsLocationBlock
 * 
 * Return type		: BOOL
 * 
 * Arguments		: gridref loc
 * 
 * Description	    : 
 * 
 */

BOOL mxmap::IsLocationBlock( mxgridref l )
{
	mxloc& mapsqr = GetAt( l );
	mxterrain* tinfo = mx->TerrainById( mapsqr.terrain );
	return tinfo->IsBlock();
}


/*
 * Function name	: map::SetLocationArmy
 * 
 * Return type		: void
 * 
 * Arguments		: gridref loc
 *                  : int number
 * 
 * Description	    : 
 * 
 */

void mxmap::SetLocationArmy( mxgridref l, u32 number )
{
	mxloc& mapsqr = GetAt( l );
	if ( number )
		mapsqr.flags|=lf_army;
	else
		mapsqr.flags&=~lf_army;
}

void mxmap::SetLocationVisible( mxgridref l, BOOL visible )
{
    mxloc& mapsqr = GetAt( l );
    if ( visible ) {
        mapsqr.flags|=lf_seen;
        CheckVisibleRange(l);
        
        if ( m_discoverymap )
            m_discoverymap->SetLocationVisible(l, true);
        
    } else
        mapsqr.flags&=~lf_seen;
}

void mxmap::SetLocationLookedAt( mxgridref l, BOOL visible )
{
    mxloc& mapsqr = GetAt( l );
    if ( visible ) {
        mapsqr.flags|=lf_looked_at;
        
        if ( m_discoverymap )
            m_discoverymap->SetLocationLookedAt(l, true);
        
        
    } else
        mapsqr.flags&=~lf_looked_at;
}

void mxmap::SetLocationVisited( mxgridref l, BOOL visible )
{
    mxloc& mapsqr = GetAt( l );
    if ( visible ) {
        mapsqr.flags|=lf_visited;
        
        if ( m_discoverymap )
            m_discoverymap->SetLocationVisited(l, true);
        
    } else
        mapsqr.flags&=~lf_visited;
}

    
void mxmap::CheckVisibleRange( mxgridref l )
{
    if ( l.x <= m_top_visible.x )
        m_top_visible.x = MAX(l.x,0);
    if ( l.x >= m_bottom_visible.x )
        m_bottom_visible.x = MIN(l.x,m_size.cx);
    if ( l.y <= m_top_visible.y )
        m_top_visible.y = MAX(l.y,0);
    if ( l.y >= m_bottom_visible.y )
        m_bottom_visible.y = MIN(l.y,m_size.cy);
}
    
/*
 * Function name	: map::SetLocationCharacter
 * 
 * Return type		: void
 * 
 * Arguments		: gridref loc
 *                  : int number
 * 
 * Description	    : 
 * 
 */

void mxmap::SetLocationCharacter( mxgridref l, u32 number )
{
	mxloc& mapsqr = GetAt( l );
	if ( number )
		mapsqr.flags|=lf_character;
	else
		mapsqr.flags&=~lf_character;
}


/*
 * Function name	: map::SetLocationSpecial
 * 
 * Return type		: void
 * 
 * Arguments		: gridref loc
 *                  : int number
 * 
 * Description	    : 
 * 
 */

void mxmap::SetLocationSpecial( mxgridref l, u32 number )
{
	mxloc& mapsqr = GetAt( l );
	if ( number )
		mapsqr.flags|=lf_special;
	else
		mapsqr.flags&=~lf_special;
}

u8 mxmap::Density(mxgridref l)
{
	u8 d = 0 ;
//	gridref l1 = l ;
//
//	loc& m = GetAt( l1 );
//
//
//	loc& m1 = GetAt( gridref(l.x,l.y-1)); if ( m1.terrain == m.terrain ) d=d+1;
//	m1 = GetAt( gridref(l.x+1,l.y)); if ( m1.terrain == m.terrain ) d=d+2;
//	m1 = GetAt( gridref(l.x,l.y+1)); if ( m1.terrain == m.terrain ) d=d+4;
//	m1 = GetAt( gridref(l.x-1,l.y)); if ( m1.terrain == m.terrain ) d=d+8;
	return d;
}



// Landscaping Location Calculation Table!           
	static mxmap::landscape_t	landscape[] = {
		{ 5, 4,  86, 64, 7 },
		{ 4, 5, 114, 64, 7 },
		{ 6, 2,  41, 64, 7 },
		{ 6, 1,  21, 64, 7 },
		{ 6,-1, -21, 64, 7 },
		{ 6, 0,   0, 64, 7 },
		{ 5, 3,  69, 63, 7 },
		{ 3, 5, 131, 63, 7 },
		{ 4, 4, 100, 63, 7 },
		{ 5, 2,  48, 63, 6 },
		{ 5, 1,  25, 63, 6 },
		{ 5,-1, -25, 63, 6 },
		{ 5, 0,   0, 62, 6 },
		{ 4, 3,  82, 62, 6 },
		{ 3, 4, 118, 62, 6 },
		{ 4, 2,  59, 62, 6 },
		{ 3, 3, 100, 61, 5 },
		{ 4, 1,  31, 61, 5 },
		{ 4,-1, -31, 61, 5 },
		{ 4, 0,   0, 60, 5 },
		{ 3, 2,  75, 59, 5 },
		{ 2, 3, 125, 59, 5 },
		{ 3, 1,  41, 58, 4 },
		{ 3,-1, -41, 58, 4 },
		{ 3, 0,   0, 57, 4 },
		{ 2, 2, 100, 57, 4 },
		{ 2, 1,  59, 53, 3 },
		{ 1, 2, 141, 53, 3 },
		{ 2, 0,   0, 51, 2 },
		{ 1, 1, 100, 43, 1 },
		{ 1, 0,   0, 32, 0 },
};
//LF32A DB     005h,004h,056h,000h,040h,007h
//		DB     004h,005h,072h,000h,040h,007h
//		DB     006h,002h,029h,000h,040h,007h
//		DB     006h,001h,015h,000h,040h,007h
//		DB     006h,0FFh,0EBh,0FFh,040h,007h
//		DB     006h,000h,000h,000h,040h,007h
//		DB     005h,003h,045h,000h,03Fh,007h
//		DB     003h,005h,083h,000h,03Fh,007h
//		DB     004h,004h,064h,000h,03Fh,007h
//		DB     005h,002h,030h,000h,03Fh,006h
//		DB     005h,001h,019h,000h,03Fh,006h
//		DB     005h,0FFh,0E7h,0FFh,03Fh,006h
//		DB     005h,000h,000h,000h,03Eh,006h
//		DB     004h,003h,052h,000h,03Eh,006h
//		DB     003h,004h,076h,000h,03Eh,006h
//		DB     004h,002h,03Bh,000h,03Eh,006h
//		DB     003h,003h,064h,000h,03Dh,005h
//		DB     004h,001h,01Fh,000h,03Dh,005h
//		DB     004h,0FFh,0E1h,0FFh,03Dh,005h
//		DB     004h,000h,000h,000h,03Ch,005h
//		DB     003h,002h,04Bh,000h,03Bh,005h
//		DB     002h,003h,07Dh,000h,03Bh,005h
//		DB     003h,001h,029h,000h,03Ah,004h
//		DB     003h,0FFh,0D7h,0FFh,03Ah,004h
//		DB     003h,000h,000h,000h,039h,004h
//		DB     002h,002h,064h,000h,039h,004h
//		DB     002h,001h,03Bh,000h,035h,003h
//		DB     001h,002h,08Dh,000h,035h,003h
//		DB     002h,000h,000h,000h,033h,002h
//		DB     001h,001h,064h,000h,02Bh,001h
//		DB     001h,000h,000h,000h,020h,000h

//L9843 DB 005h,004h,056h,000h,048h,005h
//      DB 004h,005h,072h,000h,048h,005h
//      DB 006h,002h,029h,000h,048h,005h
//      DB 006h,001h,015h,000h,048h,005h
//      DB 006h,0FFh,0EBh,0FFh,048h,005h
//      DB 006h,000h,000h,000h,048h,005h
//      DB 005h,003h,045h,000h,047h,005h
//      DB 003h,005h,083h,000h,047h,005h
//      DB 004h,004h,064h,000h,047h,005h
//      DB 005h,002h,030h,000h,047h,005h
//      DB 005h,001h,019h,000h,047h,005h
//      DB 005h,0FFh,0E7h,0FFh,047h,005h
//      DB 005h,000h,000h,000h,046h,005h
//      DB 004h,003h,052h,000h,046h,005h
//      DB 003h,004h,076h,000h,046h,005h
//      DB 004h,002h,03Bh,000h,046h,005h
//      DB 003h,003h,064h,000h,045h,004h
//      DB 004h,001h,01Fh,000h,045h,004h
//      DB 004h,0FFh,0E1h,0FFh,045h,004h
//      DB 004h,000h,000h,000h,044h,004h
//      DB 003h,002h,04Bh,000h,043h,004h
//      DB 002h,003h,07Dh,000h,043h,004h
//      DB 003h,001h,029h,000h,042h,003h
//      DB 003h,0FFh,0D7h,0FFh,042h,003h
//      DB 003h,000h,000h,000h,041h,003h
//      DB 002h,002h,064h,000h,041h,003h
//      DB 002h,001h,03Bh,000h,03Dh,002h
//      DB 001h,002h,08Dh,000h,03Dh,002h
//      DB 002h,000h,000h,000h,03Bh,002h
//      DB 001h,001h,064h,000h,033h,001h
//      DB 001h,000h,000h,000h,028h,000h

	/*
	static mxmap::landscape_t	landscape_ddr[] = {
		{ 5, 4,  86, 72, 7 },
		{ 4, 5, 114, 72, 7 },
		{ 6, 2,  41, 72, 7 },
		{ 6, 1,  21, 72, 7 },
		{ 6,-1, -21, 72, 7 },
		{ 6, 0,   0, 72, 7 },
		{ 5, 3,  69, 71, 7 },
		{ 3, 5, 131, 71, 7 },
		{ 4, 4, 100, 71, 7 },
		{ 5, 2,  48, 71, 7 },
		{ 5, 1,  25, 71, 7 },
		{ 5,-1, -25, 71, 7 },
		{ 5, 0,   0, 70, 7 },
		{ 4, 3,  82, 70, 7 },
		{ 3, 4, 118, 70, 7 },
		{ 4, 2,  59, 70, 7 },
		{ 3, 3, 100, 69, 6 },
		{ 4, 1,  31, 69, 6 },
		{ 4,-1, -31, 69, 6 },
		{ 4, 0,   0, 68, 6 },
		{ 3, 2,  75, 67, 6 },
		{ 2, 3, 125, 67, 6 },
		{ 3, 1,  41, 66, 5 },
		{ 3,-1, -41, 66, 5 },
		{ 3, 0,   0, 65, 5 },
		{ 2, 2, 100, 65, 5 },

		{ 2, 1,  59, 61, 3 },
		{ 1, 2, 141, 61, 3 },
		{ 2, 0,   0, 59, 3 },
		{ 1, 1, 100, 51, 2 },
		{ 1, 0,   0, 40, 1 },
};

*/



// Landscaping Direction Adjustment tables           
static u8	xAdjustments[] = { 3,2,0,0,2,3,1,1,3 };
static u8	yAdjustments[] = { 1,1,3,2,0,0,2,3,1 };

static f64 masterscales[] = 
	{ 1.00, 0.70, 0.50, 0.43, 0.33, 0.25, 0.21, 0.16 };

static u32	pan_count;

#define MAX_PANORAMIC_LOCATIONS		64
static panloc_t  pan_loc_buffer[MAX_PANORAMIC_LOCATIONS];


/*
 * Function name	: map::GetPanoramic
 * 
 * Return type		: int
 * 
 * Arguments		: gridref loc
 *                  : mxdir_t dir
 *                  : panloc_t** locations
 * 
 * Description	    : 
 * 
 */

u32 mxmap::GetPanoramic ( mxgridref mxloc, mxdir_t dir, panloc_t** locations )
{
int HowLftScr;
int HowRhtScr;

	pan_count=0;

	HowLftScr = 1 ;
	HowRhtScr = 0 ;
	if ( dir&1 ) {
		HowLftScr = 2 ;
		HowRhtScr = 3 ;
	}
	for ( int ii=0; ii<31; ii++ ) {
		Pan_Location ( mxloc, ii, xAdjustments[dir],   yAdjustments[dir],   HowLftScr );
		Pan_Location ( mxloc, ii, xAdjustments[dir+1], yAdjustments[dir+1], HowRhtScr );
	}

	*locations = &pan_loc_buffer[0];
	return pan_count;
}



/*
 * Function name	: map::Pan_Location
 * 
 * Return type		: void
 * 
 * Arguments		: gridref loc
 *                  : int distance
 *                  : int xtype
 *                  : int ytype
 *                  : int scrtype
 * 
 * Description	    : 
 * 
 */

//const int landscape_window_height = 384 ;
//const int landscape_window_width = 512 ;


void mxmap::Pan_Location ( mxgridref l, int distance, int xtype, int ytype, int scrtype ) 
{
mxloc	mapsqr;
int		x,y;
    l.y = Pan_AdjustMapPos ( l.y, ytype, landscape[distance].dx, landscape[distance].dy );
    l.x = Pan_AdjustMapPos ( l.x, xtype, landscape[distance].dx, landscape[distance].dy );
	//mapsqr = GetAt ( loc ) ;
   	x = 320;
    switch ( scrtype ) {
        case 0: x += landscape[distance].xadj ;			break;
        case 1: x -= landscape[distance].xadj ;			break;
        case 2: x = (x+landscape[distance].xadj)-100 ;	break;
        case 3:	x = (x-landscape[distance].xadj)+100 ;	break;
    }

	//y = 384 - ( landscape[distance].y*2 ) - 32;
	//x = (x-256)*2 ;
	//x += 256/2;
	y = landscape[distance].y;

	pan_loc_buffer[pan_count].location = l;
	pan_loc_buffer[pan_count].size = landscape[distance].size;
	pan_loc_buffer[pan_count].screen = point(x,y);
	pan_loc_buffer[pan_count].scale = (f32)masterscales[landscape[distance].size];
	pan_count++;
}


/*
 * Function name	: map::Pan_AdjustMapPos
 * 
 * Return type		: int
 * 
 * Arguments		: int pos
 *                  : int type
 *                  : int dx
 *                  : int dy
 * 
 * Description	    : 
 * 
 */

int mxmap::Pan_AdjustMapPos ( int pos, int type, int dx, int dy )
{
    switch ( type ) {
        case 0: pos = pos + dy; break;
        case 1: pos = pos - dy; break;
        case 2: pos = pos + dx; break;
        case 3: pos = pos - dx; break;
    }
	return pos ;
}


void mxmap::CalculateVisibleArea()
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
                CheckVisibleRange(loc);
            }
        }
    }
    
    
}
 
 //    void putThingsOnMap() {
 //       int r,i;
 //       for (i = 0; i < (0xc000-0xa800); i++) {
 //           r = randomish()&3;
 //           if ( (g_map[i]&0xf) >= GATE  || (g_map[i]&0xf) == r ) {
 //               g_map[i] |= HAS_OBJECT_FLAG; // has object
 //           } else {
 //               g_map[i] &= ~HAS_OBJECT_FLAG; // no object
 //           }
 //       }
 //   }
    
    
mxthing_t mxmap::getLocationObject( const mxcharacter* c, mxgridref loc )
{
    mxloc& l = GetAt ( loc );
    
#if defined(_LOM_)
    return (mxthing_t)l.object;
#endif
    
#if defined(_DDR_)
    
    if ( !(l.flags&lf_creature) )
        return OB_NONE ;
    
    //
    if ( l.IsTunnelPassageway() ) {
        // location is a passageway
        if ( c->IsInTunnel() )
            return (mxthing_t)l.object ;
    }else{
        if ( !c->IsInTunnel() ) {
            if ( (l.terrain != TN_ICYWASTE) && (l.terrain != TN_FROZENWASTE) )
                return (mxthing_t)l.object ;
        }
        
    }
#endif
    return OB_NONE;
}


    
    
    
#if defined(_DDR_)
    

    mxthing_t location_objects[16][4] = {
        { OB_DRAGONS,   OB_SKULKRIN,    OB_WOLVES,      OB_WILDHORSES },    //For    Plains
        { OB_DRAGONS,   OB_DRAGONS,     OB_ICETROLLS,   OB_WOLVES },        //       Mountins
        { OB_DRAGONS,   OB_SKULKRIN,    OB_WOLVES,      OB_WOLVES },        //       Forest
        { OB_DRAGONS,   OB_ICETROLLS,   OB_WOLVES,      OB_WOLVES },        //       Hills
        
        { OB_DRAGONS,   OB_CLAWS,       OB_FLAMES,      OB_BLOOD },         //       Gate
        { OB_FLAMES,    OB_THORNS,      OB_BLOOD,       OB_LANGUOR },       //       Temple
        { OB_ICETROLLS, OB_WOLVES,      OB_THORNS,      OB_SPRINGS },       //       Pit
        { OB_DRAGONS,   OB_WILDHORSES,  OB_CLAWS,       OB_BLOOD },         //       Palace
        { OB_DRAGONS,   OB_WOLVES,      OB_SHELTER,     OB_SHELTER },       //       Fortress
        { OB_ICETROLLS, OB_SHELTER,     OB_SHELTER,     OB_BLOOD },         //       Hall
        { OB_SKULKRIN,  OB_WOLVES,      OB_SHELTER,     OB_SHELTER },       //       Hut
        { OB_GUIDANCE,  OB_GUIDANCE,    OB_GUIDANCE,    OB_GUIDANCE },      //       Tower
        { OB_DRAGONS,   OB_SHELTER,     OB_SHELTER,     OB_SPRINGS },       //       City
        { OB_SKULKRIN,  OB_SPRINGS,     OB_SPRINGS,     OB_SPRINGS },       //       Fountain
        { OB_DRAGONS,   OB_FLAMES,      OB_FLAMES,      OB_LANGUOR },       //       Stones
        
        { OB_DRAGONS,   OB_SKULKRIN,    OB_ICETROLLS,   OB_ICETROLLS }      //       Wastes
    };
    
    int calculateNameSeed(int x, int y) {
        x--;
        y--;
        int key = (444*((y*64)+x))%6151;
        return key;
    }
    
void mxmap::PutThingsOnMap ( void )
{
    mxgridref l;
    u32 r=0;
    int key=0;
    
    for ( int y=0; y<m_size.cy; y++ ) {
        for ( int x=0; x<m_size.cx; x++ ) {
            r =  random(3);
            key = calculateNameSeed(x,y) & 3;
            l.x=x;
            l.y=y;
            mxloc& mapsqr = GetAt( l );

            // all building refresh their thing status
            // every night
            
            // plains, mountains, forest, hills all reset randomly
            
            // TODO: should be governed by bit flag on TERRAIN
            mxterrain_t t = (mxterrain_t)mapsqr.terrain ;
            
            // remap ddr/lom
            t = mx->scenario->NormaliseTerrain(t);
            
            if ( (t >= TN_GATE || (u32)(t-TN_PLAINS2) == r) && t != TN_ICYWASTE ) {
                mapsqr.flags |= lf_creature ;
            
                // if it is a tunnel passageway
                // then use the same types
                if ( mapsqr.IsTunnelPassageway() )
                    t = TN_ICYWASTE ;
   
                mapsqr.object = location_objects[t-TN_PLAINS2][key];
                
            }else{
                mapsqr.flags &= ~lf_creature ;
                mapsqr.object = OB_NONE ;
            }
            
        }
    }

}

    
void mxmap::MoveMists ( void )
{
    u8 bottom_row[ 128 ];

    mxloc*  src=&m_data[m_size.cx*(m_size.cy-1)];
    mxloc*  dst=NULL;
    
    // copy bottom row
    for ( s32 ii=0; ii<m_size.cx; ii++ )
        bottom_row[ii] = src[ii].flags&lf_mist;
    
    dst=&m_data[(m_size.cx*m_size.cy)-1];
    src = dst - m_size.cx;
    
    int size = ((m_size.cy-1) * m_size.cx);
    
    for ( s32 ii=0; ii<size; ii++ ) {
        if ( src->flags&lf_mist)
            dst->flags|=lf_mist;
        else
            dst->flags&=~lf_mist;
        src--;
        dst--;
    }
    
    dst=m_data;
    for ( s32 ii=0; ii<m_size.cx; ii++ ) {
            if ( bottom_row[ii])
                dst->flags|=lf_mist;
            else
                dst->flags&=~lf_mist;
        dst++;
    }
    
    
}


#endif
    

BOOL mxmap::IsLocOnMap ( mxgridref mxloc ) const
{
	if ( mxloc.x < 0 || mxloc.x >= m_size.cx || mxloc.y < 0 || mxloc.y >= m_size.cy ) {
		return FALSE;
	}
	return TRUE;
}




mxloc::mxloc()
{
#if defined(_DDR_)
	terrain=TN_ICYWASTE;
#else
	terrain=TN_FROZENWASTE;
#endif
	object=OB_NONE;
	area=NONE;
	flags=0;
}

mxloc::~mxloc()
{
}

BOOL mxloc::IsInteresting() const
{
mxterrain* tinfo;
	tinfo = mx->TerrainById( terrain );
	return tinfo->IsInteresting();
}

#if defined(_DDR_)
BOOL mxloc::HasTunnelExit() const 
{
	if ( HasTunnel() && (terrain >=TN_GATE && terrain <=TN_PALACE) )
		return TRUE;
	return flags & lf_tunnel_exit ;
}

BOOL mxloc::HasTunnelEntrance() const 
{
	if ( HasTunnel() && (terrain >=TN_GATE && terrain <=TN_PALACE) )
		return TRUE;
	return flags & lf_tunnel_entrance ;
}

BOOL mxloc::IsTunnelPassageway() const
{
    mxterrain_t t = mx->scenario->NormaliseTerrain((mxterrain_t)terrain);
    return HasTunnel() && (t ==TN_PLAINS2 || t==TN_MOUNTAIN2 || t==TN_FOREST2 || t==TN_HILLS ) ;
}
#endif
    
void mxloc::Serialize ( archive& ar ) 
{
	if ( ar.IsStoring() ) {
		ar.Write( this, sizeof(mxloc) );
	}else{
		u8* data = (u8*)this;
		if ( ar.IsByteSwapping() ) {
			for ( int ii=sizeof(mxloc); ii>0; ii-- )
				ar >> data[ii-1];
		}else{
			ar.Read( this, sizeof(mxloc) );
		}
#if defined(_DDR_)
        if ( terrain == TN_FROZENWASTE )
            terrain = TN_ICYWASTE ;
#endif
	}
}

}
// naemspace tme
