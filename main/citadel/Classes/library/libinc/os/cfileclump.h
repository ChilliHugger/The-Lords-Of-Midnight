// CFileClump.h: interface for the CFileClump class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _CFILECLUMP_H_INCLUDED_
#define _CFILECLUMP_H_INCLUDED_

#include "types.h"
#include "datatypes.h"

typedef class CClumpEntry* PCLUMPENTRY;

#ifndef HANDLE
#define HANDLE	void*
#endif

class LIBAPI CClumpEntry
{
public:
	LPSTR Filename();
	int Offset() const ;
	int Size() const ;

private:
	u32	m_Name;
	u32	m_offset;
	u32	m_size;
};


typedef class CClumpHdr* PCLUMPHDR;
class CClumpHdr
{
public:
	PCLUMPENTRY Entry( int index );
	int Count() const;
	int Size() const;

private:
	u32			m_id;
	PCLUMPENTRY	m_pEntries;
	LPSTR		m_pNames;
	u32			m_Size;
	u32			m_cEntries;
};

class LIBAPI CFileClump  
{
public:
	CFileClump();
	virtual ~CFileClump();
	BOOL Open ( LPSTR filename );
	void Close();
	PCLUMPENTRY Load ( LPSTR szName ) const;
	BOOL PreCache ( LPSTR szName ) const;
	BOOL PreCache () const;
	
	int Size() const;
	u8* Address() const;
	BOOL IsOpen() const;
	int Count() const;

private:
#ifdef _WINDOWS
	HANDLE		m_hFile;
	HANDLE		m_hFileMapping;
#endif
	CClumpHdr*	m_pClumpHdr;
};


#endif //_CFILECLUMP_H_INCLUDED_
