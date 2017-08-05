
#include "../../libinc/library.h"
#ifndef _DONT_DEFINE_MFC_CLASSES

using namespace chilli::os;

////////////////////////////////////////////////////////////////////////////
// CMemFile implementation

CMemFile::CMemFile(u32 nGrowBytes)
{
//	_ASSERT(nGrowBytes <= UINT_MAX);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
	m_lpBuffer = NULL;
	m_bAutoDelete = TRUE;
}

CMemFile::CMemFile(BYTE* lpBuffer, u32 nBufferSize, u32 nGrowBytes)
{
//	_ASSERT(nGrowBytes <= UINT_MAX);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = nBufferSize;
	m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
	m_lpBuffer = lpBuffer;
	m_bAutoDelete = FALSE;
}

void CMemFile::Attach(BYTE* lpBuffer, u32 nBufferSize, u32 nGrowBytes)
{
	_ASSERT(m_lpBuffer == NULL);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = nBufferSize;
	m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
	m_lpBuffer = lpBuffer;
	m_bAutoDelete = FALSE;
}

BYTE* CMemFile::Detach()
{
	BYTE* lpBuffer = m_lpBuffer;
	m_lpBuffer = NULL;
	m_nFileSize = 0;
	m_nBufferSize = 0;
	m_nPosition = 0;

	return lpBuffer;
}

CMemFile::~CMemFile()
{
	// Close should have already been called, but we check anyway
	if (m_lpBuffer)
		Close();
	_ASSERT(m_lpBuffer == NULL);

	m_nGrowBytes = 0;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
}

BYTE* CMemFile::Alloc(u32 nBytes)
{
	return (BYTE*)malloc(nBytes);
}

BYTE* CMemFile::Realloc(BYTE* lpMem, u32 nBytes)
{
	return (BYTE*)realloc(lpMem, nBytes);
}

//#pragma intrinsic(memcpy)
BYTE* CMemFile::Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource,
	u32 nBytes)
{
	_ASSERT(lpMemTarget != NULL);
	_ASSERT(lpMemSource != NULL);

	//_ASSERT(AfxIsValidAddress(lpMemTarget, nBytes));
	//_ASSERT(AfxIsValidAddress(lpMemSource, nBytes, FALSE));

	return (BYTE*)memcpy(lpMemTarget, lpMemSource, nBytes);
}
//#pragma function(memcpy)

void CMemFile::Free(BYTE* lpMem)
{
	_ASSERT(lpMem != NULL);

	free(lpMem);
}

u32 CMemFile::GetPosition() const
{
	////_ASSERT_VALID(this);
	return m_nPosition;
}

void CMemFile::GrowFile(u32 dwNewLen)
{
	////_ASSERT_VALID(this);

	if (dwNewLen > m_nBufferSize)
	{
		// grow the buffer
		u32 dwNewBufferSize = m_nBufferSize;

		// watch out for buffers which cannot be grown!
		_ASSERT(m_nGrowBytes != 0);
		//if (m_nGrowBytes == 0)
		//	AfxThrowMemoryException();

		// determine new buffer size
		while (dwNewBufferSize < dwNewLen)
			dwNewBufferSize += m_nGrowBytes;

		// allocate new buffer
		BYTE* lpNew;
		if (m_lpBuffer == NULL)
			lpNew = Alloc(dwNewBufferSize);
		else
			lpNew = Realloc(m_lpBuffer, dwNewBufferSize);

//		if (lpNew == NULL)
//			AfxThrowMemoryException();

		m_lpBuffer = lpNew;
		m_nBufferSize = dwNewBufferSize;
	}
	////_ASSERT_VALID(this);
}

u32 CMemFile::GetLength() const
{
   //_ASSERT_VALID(this);

   return m_nFileSize;
}

BOOL CMemFile::SetLength(u32 dwNewLen)
{
	//_ASSERT_VALID(this);

//#ifdef WIN32
//   if (dwNewLen > ULONG_MAX)
//	  AfxThrowMemoryException();
//#endif  // WIN32
	if (dwNewLen > m_nBufferSize)
		GrowFile((u32)dwNewLen);

	if (dwNewLen < m_nPosition)
		m_nPosition = (u32)dwNewLen;

	m_nFileSize = (u32)dwNewLen;
	//_ASSERT_VALID(this);

	return TRUE;
}

u32 CMemFile::Read(void* lpBuf, u32 nCount)
{
	//_ASSERT_VALID(this);

	if (nCount == 0)
		return 0;

	_ASSERT(lpBuf != NULL);
	//_ASSERT(AfxIsValidAddress(lpBuf, nCount));

	if (m_nPosition > m_nFileSize)
		return 0;

	u32 nRead;
	if (m_nPosition + nCount > m_nFileSize)
		nRead = (u32)(m_nFileSize - m_nPosition);
	else
		nRead = nCount;

	Memcpy((BYTE*)lpBuf, (BYTE*)m_lpBuffer + m_nPosition, nRead);
	m_nPosition += nRead;

	//_ASSERT_VALID(this);

	return nRead;
}

BOOL CMemFile::Write(const void* lpBuf, u32 nCount)
{
	//_ASSERT_VALID(this);

	if (nCount == 0)
		return TRUE;

	_ASSERT(lpBuf != NULL);
	//_ASSERT(AfxIsValidAddress(lpBuf, nCount, FALSE));

	if (m_nPosition + nCount > m_nBufferSize)
		GrowFile(m_nPosition + nCount);

	_ASSERT(m_nPosition + nCount <= m_nBufferSize);

	Memcpy((BYTE*)m_lpBuffer + m_nPosition, (BYTE*)lpBuf, nCount);

	m_nPosition += nCount;

	if (m_nPosition > m_nFileSize)
		m_nFileSize = m_nPosition;

	//_ASSERT_VALID(this);
	return TRUE;
}

u32 CMemFile::Seek(u32 lOff, u32 nFrom)
{
	//_ASSERT_VALID(this);
	_ASSERT(nFrom == begin || nFrom == end || nFrom == current);

	LONG lNewPos = m_nPosition;

	if (nFrom == begin)
		lNewPos = lOff;
	else if (nFrom == current)
		lNewPos += lOff;
	else if (nFrom == end)
		lNewPos = m_nFileSize + lOff;
	else
		return m_nPosition;

//	if ((lNewPos < 0) || (lNewPos > LONG_MAX))
//		AfxThrowFileException(CFileException::badSeek);

	m_nPosition = (u32)lNewPos;

	//_ASSERT_VALID(this);
	return m_nPosition;
}

BOOL CMemFile::Flush()
{
	//_ASSERT_VALID(this);
	return TRUE;
}

void CMemFile::Close()
{
	//_ASSERT((m_lpBuffer == NULL && m_nBufferSize == 0) ||
	//	!m_bAutoDelete || AfxIsValidAddress(m_lpBuffer, (UINT)m_nBufferSize, FALSE));
	//_ASSERT(m_nFileSize <= m_nBufferSize);

	m_nGrowBytes = 0;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nFileSize = 0;
	if (m_lpBuffer && m_bAutoDelete)
		Free(m_lpBuffer);
	m_lpBuffer = NULL;
}

void CMemFile::Abort()
{
	//_ASSERT_VALID(this);

	Close();
}


BOOL CMemFile::IsOpen() const 
{ 
	return m_lpBuffer!=NULL; 
}


u32 CMemFile::ReadLine(void* lpBuf, u32 max)
{
	_ASSERT(lpBuf != NULL);
	//_ASSERT(AfxIsValidAddress(lpBuf, nCount));

	if (m_nPosition > m_nFileSize)
		return 0;

	u8* buffer = (u8*)lpBuf ;

	u32 nRead = (u32)(m_nFileSize - m_nPosition);
	

	u8 ch=0;
	u32 count=0;
	while ( nRead-- ) {
		if ( count == max-1 )
			break;
		ch = m_lpBuffer[m_nPosition++];
		if ( ch == '\n' )
			break;
		*buffer++ = ch ;
		count++;
	}
	*buffer++ = '\0';

	return count;
}

BOOL CMemFile::Eof() const
{
	if (m_nPosition >= m_nFileSize)
		return TRUE;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
#endif //_DONT_DEFINE_MFC_CLASSES
