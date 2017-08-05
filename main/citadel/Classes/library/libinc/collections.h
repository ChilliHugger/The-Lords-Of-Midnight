#ifndef _COLLECTIONS_H_INCLUDED_
#define _COLLECTIONS_H_INCLUDED_

#include "chilli.h"

namespace chilli {

#define COLLECTION_PAGE_SIZE	16
#define MINIMUM_COLLECTION_SIZE	COLLECTION_PAGE_SIZE


	using namespace chilli::types ;

	namespace collections {
		
		typedef int ( * PFNSORTFUNCTION ) ( const void *, const void * ) ;
		
		
		template <class T>
		class base
		{
		public:
			base(void);
			base( u32 count );
			virtual ~base(void);
			BOOL Create( u32 count );
			void Clear(); 
			u32 Count() const;

			//virtual void Serialize( lib::archive& ar );
			T Get( u32 nSubscript ) const;
			T& GetAt( u32 nSubscript );
			T& operator[]( u32 nSubscript )				{ return GetAt(nSubscript); }
			void operator += ( T o )						{ Add(o); }
			void operator += ( const base& c )				{ Add(c); }
			BOOL Add ( T o );
			BOOL Add ( const base& c );
			BOOL Insert ( u32 nSubscript, T o );
			BOOL Remove ( T o );
			void Sort ( PFNSORTFUNCTION sort);

			void operator = ( const base& src );
			void Destroy( void );
			u32 Compact( void );
			BOOL Resize ( u32 newsize );

			BOOL Delete ( u32 nSubscript );

			T* RawData() const							{ return m_data; }

			int IsInList( T type ) const	;
			void Jumble ( void );
			void Concat ( void );

		protected:
			u32				m_max;
			u32				m_used;
			T*				m_data;
		};

			template <class T>
			base<T>::base(void)
			{
				m_max = 0 ;
				m_used = 0;
				m_data=NULL;
			}

			template <class T>
			base<T>::~base(void)
			{
				Destroy();
			}
		
			template <class T>
			void base<T>::Jumble ( void )
			{
				//JumbleArray ( (int*)m_data, Count() );
				// jumble them up
				for ( int ii=0; ii<Count(); ii++ ) {
					int a=ii; 
					int b= randomno::instance.get(1,Count())-1;
					T temp;
					temp = m_data[a];
					m_data[a]=m_data[b];
					m_data[b]=temp;
				}

			}

			template <class T>
			void base<T>::Concat ( void )
			{
				//m_used = ConcatArray ( (int*)m_data, Count() );
			s32 ii=0;
			u32 count=0;

				BOOL Done = FALSE;
				while( !Done )
				{
					Done = TRUE;
					for(  ii = 0; ii < (s32)Count()-1; ii++ ) {
						if( m_data[ii] == 0 && m_data[ii+1] ) {
							m_data[ii] = m_data[ii+1];
							m_data[ii+1] = NULL;
							Done = FALSE;
						};
					};
				};

				for ( ii=0, count=0; ii<(s32)Count(); ii++, count++ ) {
					if ( m_data[ii] == 0 )
						break;

				}
				m_used = count;

			}


			template <class T>
			BOOL base<T>::Remove ( T o )
			{
				int pos = IsInList(o);
				if ( pos == 0 ) return FALSE ;
				return Delete(pos-1);
			}

			template <class T>
			void base<T>::operator = ( const base& src )
			{
				Clear();
				if ( src.Count() ) {
					Create(src.Count());
					for ( int ii=0; ii<src.Count(); ii++ ) {
						m_data[ii] = src.m_data[ii];
						m_used++;
					}
				}
			}

			template <class T>
			BOOL base<T>::Create( u32 count )
			{
				Clear();
				m_max = MAX(count,MINIMUM_COLLECTION_SIZE);
				m_used = count ;
				m_data = new T[ m_max ];
				memset ( m_data, 0, sizeof(T)*m_max );
				return TRUE ;
			}

			template <class T>
			u32 base<T>::Count() const
			{ 
				return m_used;
			}

			template <class T>
			u32 base<T>::Compact( void )
			{
				Resize(Count());
				return Count();
			}

			template <class T>
			BOOL base<T>::Resize ( u32 newsize )
			{
				if ( newsize == m_max )
					return TRUE;

				if ( newsize == 0 ) {
					// lose old array
					SAFEDELETEARRAY(m_data);
					m_max=0;
					m_used=0;
				}

				// allocate new size
				T* elements = new T [ newsize ];
				memset ( elements, 0, sizeof(T)*newsize );

				// copy the original
				for ( u32 ii=0; ii<MIN(newsize,m_used); ii++ ) {
					elements[ii] = m_data[ii];
				}
				
				// lose old array
				SAFEDELETEARRAY(m_data);

				// new data
				m_data = elements ;
				m_max = newsize;
				m_used = MIN( m_used, m_max );

				return TRUE;

			}

			template <class T>
			BOOL base<T>::Insert ( u32 nSubscript, T o )
			{
				if ( Count() < nSubscript )
					return FALSE;

				if ( m_used == m_max ) 
					Add(0);

				int elements = Count()-nSubscript ;

				memmove ( &m_data[nSubscript+1], &m_data[nSubscript], sizeof(T) * elements );
				m_data[nSubscript]=0;

				m_used++;

				return TRUE;
			}

			template <class T>
			BOOL base<T>::Delete ( u32 nSubscript )
			{
				if ( /*nSubscript<0 ||*/ nSubscript>=Count() )
					return FALSE;

				if ( nSubscript == Count()-1 ) {
					m_data[nSubscript]=0;
					m_used--;
					return TRUE;
				};

				int elements = Count()-nSubscript-1 ;

				memmove ( &m_data[nSubscript], &m_data[nSubscript+1], sizeof(T) * elements );

				m_used--;

				return TRUE;
			}

			template <class T>
			BOOL base<T>::Add ( T o )
			{
				if ( m_used == m_max ) {
					// add a new page				
					int size = m_max + COLLECTION_PAGE_SIZE ;
					size = (size / COLLECTION_PAGE_SIZE) * COLLECTION_PAGE_SIZE ;
					Resize( size );
				}
				m_data[m_used++] = o;
				return TRUE;
			}

			template <class T>
			BOOL base<T>::Add ( const base& c )
			{
				int count = c.Count();
				if ( count == 0 )
					return TRUE;

				if ( m_used+count >= m_max ) {
					int size = m_used + count + COLLECTION_PAGE_SIZE ;
					size = (size / COLLECTION_PAGE_SIZE) * COLLECTION_PAGE_SIZE ;
					Resize( size );
				}

				memcpy ( &m_data[m_used], &c.m_data[0], sizeof(T)*count );
				m_used += count;
				return TRUE;
			}

			template <class T>
			T& base<T>::GetAt( u32 nSubscript )
			{
			static T iErr;
				if ( /*nSubscript>=0 && */ nSubscript<Count() )
					return m_data[ nSubscript ];
				return iErr;
			}

			template <class T>
			T base<T>::Get( u32 nSubscript ) const
			{
			static T iErr;
				if ( /*nSubscript>=0 &&*/ nSubscript<Count() )
					return m_data[ nSubscript ];
				return iErr;
			}

			template <class T>
			void base<T>::Clear( void )
			{
				m_used=0;
				m_max=0;
				SAFEDELETEARRAY(m_data);
			}

			//template <class T>
			//void base<T>::Serialize( archive& ar )
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

			template <class T>
			void base<T>::Destroy( )
			{
				Clear();
			}

			template <class T>
			BOOL base<T>::IsInList( T type ) const	
			{
				for ( u32 ii=0; ii<Count(); ii++ )
					if ( m_data[ii] == type )
						return ii+1;

				return 0;
			}
		
			template <class T>
			void base<T>::Sort ( PFNSORTFUNCTION sort)
			{
				qsort ( m_data, Count(), sizeof(m_data), sort );
			}



		//class c_ptr :	public base
		//{
		//public:
		//	c_ptr(void);
		//	~c_ptr(void);
		//	virtual void Serialize( lib::archive& ar );
		//	void* Get( u32 nSubscript )					{ return (void*)base::Get(nSubscript); }
		//	void*& GetAt( u32 nSubscript )				{ return (void*&)base::GetAt(nSubscript); }
		//	void*& operator[]( u32 nSubscript )			{ return (void*&)base::GetAt(nSubscript); }
		//	void operator += ( void* o )				{ base::Add((u32)o); }
		//	void operator += ( const c_ptr& c )			{ base::Add((base&)c); }
		//	BOOL Add ( void* o )						{ return base::Add((u32)o); }
		//	BOOL Insert ( u32 nSubscript, void* o )		{ return base::Insert(nSubscript,(u32)o); }
		//	BOOL Remove ( void* o )						{ return base::Remove((u32)o); }

		//};

		//// 
		//class c_float :	public base
		//{
		//public:
		//	c_float(void);
		//	~c_float(void);
		//	virtual void Serialize( lib::archive& ar );
		//	f32& GetAt( u32 nSubscript )				{ return (f32&)base::GetAt(nSubscript); }
		//	f32& operator[]( u32 nSubscript )			{ return (f32&)base::GetAt(nSubscript); }
		//	void operator += ( const c_float& c )		{ base::Add((base&)c); }
		//	f32 Get( u32 nSubscript );
		//	void operator += ( f32 o )					{ Add(o); }
		//	BOOL Add ( f32 o );
		//	BOOL Insert ( u32 nSubscript, f32 o );
		//	BOOL Remove ( f32 o );
		//	int IsInList( f32 type ) const;

		//};

		//class c_point :	public base
		//{
		//public:
		//	c_point(void);
		//	~c_point(void);
		//	virtual void Serialize( lib::archive& ar );
		//	void* Get( u32 nSubscript )					{ return (void*)base::Get(nSubscript); }
		//	void*& GetAt( u32 nSubscript )				{ return (void*&)base::GetAt(nSubscript); }
		//	void*& operator[]( u32 nSubscript )			{ return (void*&)base::GetAt(nSubscript); }
		//	void operator += ( void* o )				{ base::Add((u32)o); }
		//	void operator += ( const c_point& c )			{ base::Add((base&)c); }
		//	BOOL Add ( void* o )						{ return base::Add((u32)o); }
		//	BOOL Insert ( u32 nSubscript, void* o )		{ return base::Insert(nSubscript,(u32)o); }
		//	BOOL Remove ( void* o )						{ return base::Remove((u32)o); }

		//};

		// idt collection
		//class c_idt :	public base
		//{
		//public:
		//	c_idt(void);
		//	~c_idt(void);
		//	virtual void Serialize( lib::archive& ar );
		//};
		// class Idt collection

		typedef base<point>		c_point ;
		typedef base<f32>		c_float ;
		typedef base<void*>		c_ptr ;
		typedef base<string>	c_string ;
		typedef base<u32>		c_u32 ;
		typedef base<s32>		c_s32 ;
		typedef base<mxid>		c_mxid ;

	}

}

#endif //
