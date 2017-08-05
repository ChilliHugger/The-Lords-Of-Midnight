#include "../../libinc/library.h"

namespace tme {

	namespace collections {


			//c_ptr::c_ptr(void)
			//{
			//	m_max = 0 ;
			//	m_used = 0;
			//	m_data=NULL;
			//}

			//c_ptr::~c_ptr(void)
			//{
			//	Destroy();
			//}

			//void c_ptr::Serialize( archive& ar )
			//{
			//}


			//c_idt::c_idt(void)
			//{
			//	m_max = 0 ;
			//	m_used = 0;
			//	m_data=NULL;
			//}

			//c_idt::~c_idt(void)
			//{
			//	Destroy();
			//}

			//void c_idt::Serialize( archive& ar )
			//{
			//}


			//c_float::c_float(void)
			//{
			//	m_max = 0 ;
			//	m_used = 0;
			//	m_data=NULL;
			//}

			//c_float::~c_float(void)
			//{
			//	Destroy();
			//}

			//void c_float::Serialize( archive& ar )
			//{
			//}

			//f32 c_float::Get( u32 nSubscript )
			//{
			//	if ( nSubscript>=0 && nSubscript<Count() )
			//		return ((f32*)m_data)[ nSubscript ];
			//	return 0;
			//}
			//
			//BOOL c_float::Add ( f32 o )
			//{
			//	if ( m_used == m_max ) {
			//		// add a new page				
			//		int size = m_max + COLLECTION_PAGE_SIZE ;
			//		size = (size / COLLECTION_PAGE_SIZE) * COLLECTION_PAGE_SIZE ;
			//		Resize( size );
			//	}
			//	((f32*)m_data)[m_used++] = o;
			//	return TRUE;
			//}

			//int c_float::IsInList( f32 type ) const	
			//{
			//	for ( int ii=0; ii<Count(); ii++ )
			//		if ( m_data[ii] == type )
			//			return ii+1;

			//	return 0;
			//}

			//BOOL c_float::Remove ( f32 o )
			//{
			//	int pos = IsInList(o);
			//	if ( pos == 0 ) return FALSE ;
			//	return Delete(pos-1);
			//}

			//BOOL c_float::Insert ( u32 nSubscript, f32 o )
			//{
			//	if ( Count() < nSubscript )
			//		return FALSE;

			//	if ( m_used == m_max ) 
			//		Add(0);

			//	int elements = Count()-nSubscript ;

			//	memmove ( &m_data[nSubscript+1], &m_data[nSubscript], sizeof(u32) * elements );
			//	m_data[nSubscript]=0;

			//	return TRUE;
			//}




			//base::base(void)
			//{
			//	m_max = 0 ;
			//	m_used = 0;
			//	m_data=NULL;
			//}

			//base::~base(void)
			//{
			//	Destroy();
			//}

			//void base::Jumble ( void )
			//{
			//	JumbleArray ( (int*)m_data, Count() );
			//}

			//void base::Concat ( void )
			//{
			//	m_used = ConcatArray ( (int*)m_data, Count() );
			//}


			//BOOL base::Remove ( u32 o )
			//{
			//	int pos = IsInList(o);
			//	if ( pos == 0 ) return FALSE ;
			//	return Delete(pos-1);
			//}

			//void base::operator = ( const base& src )
			//{
			//	Clear();
			//	if ( src.Count() ) {
			//		Create(src.Count());
			//		for ( int ii=0; ii<src.Count(); ii++ ) {
			//			m_data[ii] = src.m_data[ii];
			//			m_used++;
			//		}
			//	}
			//}

			////BOOL base::CreateIdtCollection ( c_idt& obj )
			////{
			////	obj.Create( Count() );
			////	for ( int ii=0; ii<Count(); ii++ )
			////		obj[ii] = m_data[ii]->SafeIdt();
			////
			////	return TRUE;
			////}

			//BOOL base::Create( u32 count )
			//{
			//	Clear();
			//	m_max = MAX(count,MINIMUM_COLLECTION_SIZE);
			//	m_used = count ;
			//	m_data = new u32[ m_max ];
			//	memset ( m_data, 0, sizeof(u32)*m_max );
			//	return TRUE ;
			//}

			//u32 base::Count() const
			//{ 
			//	return m_used;
			//}

			//u32 base::Compact( void )
			//{
			//	Resize(Count());
			//	return Count();
			//}

			//BOOL base::Resize ( u32 newsize )
			//{
			//	if ( newsize == m_max )
			//		return TRUE;

			//	if ( newsize == 0 ) {
			//		// lose old array
			//		SAFEDELETEARRAY(m_data);
			//		m_max=0;
			//		m_used=0;
			//	}

			//	// allocate new size
			//	u32* elements = new u32 [ newsize ];
			//	memset ( elements, 0, sizeof(u32)*newsize );

			//	// copy the original
			//	for ( int ii=0; ii<MIN(newsize,m_used); ii++ ) {
			//		elements[ii] = m_data[ii];
			////		m_data[ii]=NULL;
			//	}
			//	
			//	// lose old array
			//	SAFEDELETEARRAY(m_data);

			//	// new data
			//	m_data = elements ;
			//	m_max = newsize;
			//	m_used = MIN( m_used, m_max );

			//	return TRUE;

			//}

			//BOOL base::Insert ( u32 nSubscript, u32 o )
			//{
			//	if ( Count() < nSubscript )
			//		return FALSE;

			//	if ( m_used == m_max ) 
			//		Add(0);

			//	int elements = Count()-nSubscript ;

			//	memmove ( &m_data[nSubscript+1], &m_data[nSubscript], sizeof(u32) * elements );
			//	m_data[nSubscript]=0;

			//	m_used++;

			//	return TRUE;
			//}

			//BOOL base::Delete ( u32 nSubscript )
			//{
			//	if ( nSubscript<0 || nSubscript>=Count() )
			//		return FALSE;

			//	if ( nSubscript == Count()-1 ) {
			//		m_data[nSubscript]=0;
			//		m_used--;
			//		return TRUE;
			//	};

			//	int elements = Count()-nSubscript-1 ;

			//	memcpy ( &m_data[nSubscript], &m_data[nSubscript+1], sizeof(u32) * elements );

			//	m_used--;

			//	return TRUE;
			//}

			//BOOL base::Add ( u32 o )
			//{
			//	if ( m_used == m_max ) {
			//		// add a new page				
			//		int size = m_max + COLLECTION_PAGE_SIZE ;
			//		size = (size / COLLECTION_PAGE_SIZE) * COLLECTION_PAGE_SIZE ;
			//		Resize( size );
			//	}
			//	m_data[m_used++] = o;
			//	return TRUE;
			//}

			//BOOL base::Add ( const base& c )
			//{
			//	int count = c.Count();
			//	if ( count == 0 )
			//		return TRUE;

			//	if ( m_used+count >= m_max ) {
			//		int size = m_used + count + COLLECTION_PAGE_SIZE ;
			//		size = (size / COLLECTION_PAGE_SIZE) * COLLECTION_PAGE_SIZE ;
			//		Resize( size );
			//	}

			//	memcpy ( &m_data[m_used], &c.m_data[0], sizeof(u32)*count );
			//	m_used += count;
			//	return TRUE;
			//}



			//u32& base::GetAt( u32 nSubscript )
			//{
			//static u32 iErr = NULL;

			//	if ( nSubscript>=0 && nSubscript<Count() )
			//		return m_data[ nSubscript ];
			//	return iErr;
			//}

			//u32 base::Get( u32 nSubscript ) const
			//{
			//	if ( nSubscript>=0 && nSubscript<Count() )
			//		return m_data[ nSubscript ];
			//	return 0;
			//}

			//void base::Clear( void )
			//{
			//	m_used=0;
			//	m_max=0;
			//	SAFEDELETEARRAY(m_data);
			//}

			//void base::Serialize( archive& ar )
			//{
			//int ii;

			//	if ( ar.IsStoring() ) {
			//		ar << Count();
			//		for ( ii=0; ii<Count(); ii++ ) {
			//			ar << m_data[ii];
			//		}
			//	}else{
			//		ar >> ii;
			//		Create(ii);
			//		for ( ii=0; ii<Count(); ii++ ) {
			//			ar >> m_data[ii];
			//		}
			//	}
			//}

			//void base::Destroy( )
			//{
			//	Clear();
			//}


			//int base::IsInList( u32 type ) const	
			//{
			//	for ( int ii=0; ii<Count(); ii++ )
			//		if ( m_data[ii] == type )
			//			return ii+1;

			//	return 0;
			//}

}
	// namespace collection
}
// namespace tme
