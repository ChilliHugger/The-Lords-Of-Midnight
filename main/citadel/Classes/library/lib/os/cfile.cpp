/*
 * FILE:    CFile.cpp
 * 
 * PROJECT: LibraryOS
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * � Copyright 1999 The Midnight Council.
 * 
 * PURPOSE: 
 * 
 * 
 */
#include "../../libinc/library.h"

#ifdef _WINDOWS
//#ifdef _WIN32


namespace chilli {

	namespace os {

		BOOL file::Open(LPCSTR lpszFileName, u32 nOpenFlags )
		{
			// CFile objects are always binary and CreateFile does not need flag
			nOpenFlags &= ~(u32)typeBinary;

			m_bCloseOnDelete = FALSE;
			m_hFile = (u32)hFileNull;

			memset( m_strFileName, 0x00, sizeof(m_strFileName) );
			_fullpath( m_strFileName, lpszFileName, MAX_PATH );

		//	_ASSERTE(shareCompat == 0);

			// map read/write mode
			_ASSERTE((modeRead|modeWrite|modeReadWrite) == 3);
			DWORD dwAccess;
			switch (nOpenFlags & 3)	{
			case modeRead:
				dwAccess = GENERIC_READ;
				break;
			case modeWrite:
				dwAccess = GENERIC_WRITE;
				break;
			case modeReadWrite:
				dwAccess = GENERIC_READ|GENERIC_WRITE;
				break;
			default:
				_ASSERTE(FALSE);  // invalid share mode
			}

			// map share mode
			DWORD dwShareMode;
			switch (nOpenFlags & 0x70)	{
			case shareCompat:       // map compatibility mode to exclusive
			case shareExclusive:
				dwShareMode = 0;
				break;
			case shareDenyWrite:
				dwShareMode = FILE_SHARE_READ;
				break;
			case shareDenyRead:
				dwShareMode = FILE_SHARE_WRITE;
				break;
			case shareDenyNone:
				dwShareMode = FILE_SHARE_WRITE|FILE_SHARE_READ;
				break;
			default:
				_ASSERTE(FALSE);  // invalid share mode?
			}

			// Note: typeText and typeBinary are used in derived classes only.

			// map modeNoInherit flag
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = (nOpenFlags & modeNoInherit) == 0;

			// map creation flags
			DWORD dwCreateFlag;
			if (nOpenFlags & modeCreate)	{
				if (nOpenFlags & modeNoTruncate)
					dwCreateFlag = OPEN_ALWAYS;
				else
					dwCreateFlag = CREATE_ALWAYS;
			}
			else
				dwCreateFlag = OPEN_EXISTING;

			// attempt file creation
			HANDLE hFile = ::CreateFile(lpszFileName, dwAccess, dwShareMode, &sa,
				dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE )
				return FALSE;
			m_hFile = (HFILE)hFile;
			m_bCloseOnDelete = TRUE;
			return TRUE;
		}

		u32 file::Read(void* lpBuf, u32 nCount)
		{
			_ASSERTE(m_hFile != (u32)hFileNull);

			if (nCount == 0)
				return 0;   // avoid Win32 "null-read"

			_ASSERTE(lpBuf != NULL);

			DWORD dwRead;
			::ReadFile((HANDLE)m_hFile, lpBuf, nCount, &dwRead, NULL);

			return (u32)dwRead;
		}

		BOOL file::Write(const void* lpBuf, u32 nCount)
		{
			_ASSERTE(m_hFile != (u32)hFileNull);

			if (nCount == 0)
				return(TRUE);     // avoid Win32 "null-write" option

			_ASSERTE(lpBuf != NULL);

			DWORD nWritten;
			BOOL bRet = ::WriteFile((HANDLE)m_hFile, lpBuf, nCount, &nWritten, NULL);

			// Win32s will not return an error all the time (usually DISK_FULL)
			if (nWritten != nCount)
				return (FALSE);

			return ( bRet );
		}

		LONG file::Seek(LONG lOff, u32 nFrom) const
		{
			_ASSERTE(m_hFile != (u32)hFileNull);
			_ASSERTE(nFrom == begin || nFrom == end || nFrom == current);
			_ASSERTE(begin == FILE_BEGIN && end == FILE_END && current == FILE_CURRENT);

			DWORD dwNew = ::SetFilePointer((HANDLE)m_hFile, lOff, NULL, (DWORD)nFrom);

			return dwNew;
		}

		DWORD file::GetPosition() const
		{
			_ASSERTE(m_hFile != (u32)hFileNull);

			DWORD dwPos = ::SetFilePointer((HANDLE)m_hFile, 0, NULL, FILE_CURRENT);

			return dwPos;
		}

		BOOL file::Flush()
		{
			if (m_hFile == (u32)hFileNull)
				return(TRUE);

			return ::FlushFileBuffers((HANDLE)m_hFile);
		}

		void file::Close()
		{
			_ASSERTE(m_hFile != (u32)hFileNull);

			BOOL bError = FALSE;
			if (m_hFile != (u32)hFileNull)
				bError = !::CloseHandle((HANDLE)m_hFile);

			m_hFile = (u32) hFileNull;
			m_bCloseOnDelete = FALSE;
			memset( m_strFileName, 0x00, sizeof(m_strFileName) );

		}

		void file::Abort()
		{
			if (m_hFile != (u32)hFileNull)	{
				// close but ignore errors
				::CloseHandle((HANDLE)m_hFile);
				m_hFile = (u32)hFileNull;
			}
			memset( m_strFileName, 0x00, sizeof(m_strFileName) );
		}

		BOOL file::SetLength(DWORD dwNewLen)
		{
			_ASSERTE(m_hFile != (u32)hFileNull);

			Seek((LONG)dwNewLen, (u32)begin);

			return ::SetEndOfFile((HANDLE)m_hFile);
		}

		DWORD file::GetLength() const
		{
		DWORD dwLen;

			dwLen = GetFileSize ( (HANDLE)m_hFile, NULL );
			/* Seek is a non const operation
			DWORD dwCur;
			CFile* pFile = (CFile*)this;
			dwCur = pFile->Seek(0L, current);
			dwLen = pFile->SeekToEnd();
			pFile->Seek(dwCur, begin);
			*/

			return dwLen;
		}


	}
	// namespace os
}
// namespace tme



#endif // WIN32
