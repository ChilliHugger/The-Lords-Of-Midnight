#ifndef _COLLECTION_H_INCLUDED
#define _COLLECTION_H_INCLUDED

#define COLLECTION_PAGE_SIZE	16
#define MINIMUM_COLLECTION_SIZE	COLLECTION_PAGE_SIZE

class LIBCLASS CCollection
{
public:
	CCollection(void);
	CCollection( u32 count );
	virtual ~CCollection(void);
	BOOL Create( u32 count );
	void Clear(); 
	u32 Count() const;

	virtual void Serialize( CArchive& ar );
	u32 Get( u32 nSubscript ) const;
	u32& GetAt( u32 nSubscript );
	u32& operator[]( u32 nSubscript )				{ return GetAt(nSubscript); }
	void operator += ( u32 o )						{ Add(o); }
	void operator += ( const CCollection& c )				{ Add(c); }
	BOOL Add ( u32 o );
	BOOL Add ( const CCollection& c );
	BOOL Insert ( u32 nSubscript, u32 o );

	void operator = ( const CCollection& src );
	void Destroy( void );
	u32 Compact( void );
	BOOL Resize ( u32 newsize );

	BOOL Delete ( u32 nSubscript );

	u32* RawData() const							{ return m_data; }

	int IsInList( u32 type ) const	;
	void Jumble ( void );
	void Concat ( void );

protected:
	u32				m_max;
	u32				m_used;
	u32*			m_data;
};

class LIBCLASS CPtrCollection :	public CCollection
{
public:
	CPtrCollection(void);
	~CPtrCollection(void);
	virtual void Serialize( CArchive& ar );
	void* Get( u32 nSubscript )					{ return (void*)CCollection::Get(nSubscript); }
	void*& operator[]( u32 nSubscript )			{ return (void*&)CCollection::GetAt(nSubscript); }
	void operator += ( void* o )				{ CCollection::Add((u32)o); }
	void operator += ( const CPtrCollection& c ){ CCollection::Add((CCollection&)c); }
	BOOL Add ( void* o )						{ return CCollection::Add((u32)o); }
	BOOL Insert ( u32 nSubscript, void* o )		{ return CCollection::Insert(nSubscript,(u32)o); }

};


#endif //_COLLECTION_H_INCLUDED
