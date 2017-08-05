#include "../../libinc/library.h"
#include <string.h>

namespace chilli {

	namespace lib {

		archive::archive()
		{
		}

		archive::archive(ARFILE pFile, u32 nMode )
		{
			Open(pFile, nMode );
		}
		  
		void archive::Open(ARFILE pFile, u32 nMode )
		{
			// initialize members not dependent on allocated buffer
			m_nMode = nMode;
			m_pFile = pFile;

			// initialize the buffer.  minimum size is 128
			m_lpBufStart = NULL;
			m_bUserBuf = FALSE;

			m_nBufSize = 0;
			m_lpBufStart = NULL;

			// force use of private buffer of minimum size
		#if defined(_BUFFERED_ARCHIVE_) 
			m_nBufSize = 4906*4;

			m_lpBufStart = new BYTE[m_nBufSize];
			memset ( m_lpBufStart, 0xfd, m_nBufSize );


			_ASSERTE(m_lpBufStart != NULL);

			m_lpBufMax = m_lpBufStart + m_nBufSize;
			m_lpBufCur = (IsLoading()) ? m_lpBufMax : m_lpBufStart;
		#endif

		}

		archive::~archive()
		{
			// Close makes m_pFile NULL. If it is not NULL, we must Close the archive
			if (m_pFile != NULL )
				Close();

			Abort();    // abort completely shuts down the archive
		}

		void archive::Abort()
		{
			// disconnect from the file
			m_pFile = NULL;

			if (!m_bUserBuf) {
				delete[] m_lpBufStart;
				m_lpBufStart = NULL;
				m_lpBufCur = NULL;
			}

		}

		void archive::Close()
		{
			Flush();
			m_pFile = NULL;
		}

		u32 archive::Read(void* lpBuf, u32 nMax)
		{
			if (nMax == 0)
				return 0;

		#if defined(_BUFFERED_ARCHIVE_) 
			// try to fill from buffer first
			u32 nMaxTemp = nMax;
			u32 nTemp = MIN(nMaxTemp, (u32)(m_lpBufMax - m_lpBufCur));
			memcpy(lpBuf, m_lpBufCur, nTemp);
			m_lpBufCur += nTemp;
			lpBuf = (BYTE*)lpBuf + nTemp;
			nMaxTemp -= nTemp;

			if (nMaxTemp != 0)
			{
				_ASSERTE(m_lpBufCur == m_lpBufMax);

				// read rest in buffer size chunks
				nTemp = nMaxTemp - (nMaxTemp % m_nBufSize);
				u32 nRead = 0;

				u32 nLeft = nTemp;
				u32 nBytes;
				do
				{
					nBytes = m_pFile->Read(lpBuf, nLeft);
					lpBuf = (BYTE*)lpBuf + nBytes;
					nRead += nBytes;
					nLeft -= nBytes;
				}
				while ((nBytes > 0) && (nLeft > 0));

				nMaxTemp -= nRead;

				// read last chunk into buffer then copy
				if (nRead == nTemp)
				{
					_ASSERTE(m_lpBufCur == m_lpBufMax);
					_ASSERTE(nMaxTemp < (u32)m_nBufSize);

					// fill buffer (similar to archive::FillBuffer, but no exception)
						u32 nLeft = MAX(nMaxTemp, (u32)m_nBufSize);
						u32 nBytes;
						BYTE* lpTemp = m_lpBufStart;
						nRead = 0;
						do
						{
							nBytes = m_pFile->Read(lpTemp, nLeft);
							lpTemp = lpTemp + nBytes;
							nRead += nBytes;
							nLeft -= nBytes;
						}
						while ((nBytes > 0) && (nLeft > 0) && nRead < nMaxTemp);

						m_lpBufCur = m_lpBufStart;
						m_lpBufMax = m_lpBufStart + nRead;

					// use first part for rest of read
					nTemp = MIN(nMaxTemp, (u32)(m_lpBufMax - m_lpBufCur));
					memcpy(lpBuf, m_lpBufCur, nTemp);
					m_lpBufCur += nTemp;
					nMaxTemp -= nTemp;
				}
			}
			return nMax - nMaxTemp;
		#else
			return m_pFile->Read(lpBuf, nMax);
		//	return fread ( lpBuf,sizeof(u8), nMax, m_pFile );
		#endif //_BUFFERED_ARCHIVE_

		}

		void archive::Write(const void* lpBuf, u32 nMax)
		{
			if (nMax == 0)
				return;

		#if defined(_BUFFERED_ARCHIVE_) 

			// copy to buffer if possible
			u32 nTemp = MIN(nMax, (u32)(m_lpBufMax - m_lpBufCur));
			memcpy(m_lpBufCur, lpBuf, nTemp);
			m_lpBufCur += nTemp;
			lpBuf = (BYTE*)lpBuf + nTemp;
			nMax -= nTemp;

			if (nMax > 0)
			{
				Flush();    // flush the full buffer

				// write rest of buffer size chunks
				nTemp = nMax - (nMax % m_nBufSize);
				m_pFile->Write(lpBuf, nTemp);
				lpBuf = (BYTE*)lpBuf + nTemp;
				nMax -= nTemp;

				// copy remaining to active buffer
				_ASSERTE(nMax < (u32)m_nBufSize);
				_ASSERTE(m_lpBufCur == m_lpBufStart);
				memcpy(m_lpBufCur, lpBuf, nMax);
				m_lpBufCur += nMax;
			}
		#else

			m_pFile->Write(lpBuf, nMax);
		//	fwrite ( lpBuf,sizeof(u8), nMax, m_pFile );

		#endif // _BUFFERED_ARCHIVE_

		}

		void archive::Flush()
		{
		#if defined(_BUFFERED_ARCHIVE_)
			if (IsLoading()) {
				// unget the characters in the buffer, seek back unused amount
				if (m_lpBufMax - m_lpBufCur != 0)
					m_pFile->Seek(-(m_lpBufMax - m_lpBufCur), os::file::current);
				m_lpBufCur = m_lpBufMax;    // empty
			} else {
				if (m_lpBufStart == NULL || m_lpBufCur != m_lpBufStart) 	{
					// write out the current buffer to file
					m_pFile->Write(m_lpBufStart, m_lpBufCur - m_lpBufStart);
					m_lpBufCur = m_lpBufStart;
					memset ( m_lpBufStart, 0xfd, m_nBufSize );
				}
			}
		#endif // _BUFFERED_ARCHIVE_
		}

		void archive::FillBuffer(u32 nBytesNeeded)
		{
		#if defined(_BUFFERED_ARCHIVE_) 
			u32 nUnused = m_lpBufMax - m_lpBufCur;
			u32 nTotalNeeded = ((u32)nBytesNeeded) + nUnused;

			// fill up the current buffer from file
			if (m_lpBufCur > m_lpBufStart)	{
				// copy unused
				if ((int)nUnused > 0) {
					memmove(m_lpBufStart, m_lpBufCur, nUnused);
					m_lpBufCur = m_lpBufStart;
					m_lpBufMax = m_lpBufStart + nUnused;
				}

				// read to satisfy nBytesNeeded or nLeft if possible
				u32 nRead = nUnused;
				u32 nLeft = m_nBufSize-nUnused;
				u32 nBytes;
				BYTE* lpTemp = m_lpBufStart + nUnused;
				do
				{
					nBytes = m_pFile->Read(lpTemp, nLeft);
					lpTemp = lpTemp + nBytes;
					nRead += nBytes;
					nLeft -= nBytes;
				}
				while (nBytes > 0 && nLeft > 0 && nRead < nBytesNeeded);

				m_lpBufCur = m_lpBufStart;
				m_lpBufMax = m_lpBufStart + nRead;
			}
		#endif // _BUFFERED_ARCHIVE_
		}


		/////////////////////////////////////////////////////////////////////////////

		archive& archive::operator<<(u8 by)
		{ 
		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u8) > m_lpBufMax) Flush();

			*(UNALIGNED u8*)m_lpBufCur = by; 
			
			m_lpBufCur += sizeof(u8); 
		#else
			Write ( &by, sizeof(u8) );
		#endif

			return *this;
		}

		archive& archive::operator<<(u16 w)
		{
			if ( IsByteSwapping() )
				w = lib::u16Swap(w);

		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u16) > m_lpBufMax) Flush();
			*(UNALIGNED u16*)m_lpBufCur = w;
			m_lpBufCur += sizeof(u16); 
		#else
			Write ( &w, sizeof(u16) );
		#endif // _BUFFERED_ARCHIVE_

			return *this;
		}

		archive& archive::operator<<(u32 l)
		{ 
			if ( IsByteSwapping() )
				l=lib::u32Swap(l);

		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u32) > m_lpBufMax) Flush();
			*(UNALIGNED u32*)m_lpBufCur = l;
			m_lpBufCur += sizeof(u32); 
		#else
			Write ( &l, sizeof(u32) );
		#endif // _BUFFERED_ARCHIVE_

			return *this;
		}

		archive& archive::operator<<(u64 l)
		{ 
			if ( IsByteSwapping() )
				l=lib::u64Swap(l);

		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u64) > m_lpBufMax) Flush();
			*(UNALIGNED u64*)m_lpBufCur = l;
			m_lpBufCur += sizeof(u64); 
		#else
			Write ( &l, sizeof(u64) );
		#endif // _BUFFERED_ARCHIVE_

			return *this;
		}

		archive& archive::operator<<(long l)
		{ 
			_ASSERTE(sizeof(long) == sizeof(u32));
			return operator<<((u32) l);
		}
		 
		archive& archive::operator<<(f32 f)
		{
			_ASSERTE(sizeof(f32) == sizeof(u32));
			//u32 l;
			//memcpy( &l, &f, sizeof(u32) );
			return operator<<((u32&)f);
			//return ar;
		}

		archive& archive::operator<<(f64 f)
		{
			_ASSERTE(sizeof(f64) == sizeof(u64));
			//u64 l;
			//memcpy( &l, &f, sizeof(u64) );
			return operator<<((u64&)f);
			//return ar;
		}

		archive& archive::operator>>(u8& by)
		{
		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u8) > m_lpBufMax)
					FillBuffer(sizeof(u8) - (u32)(m_lpBufMax - m_lpBufCur));
			by = *(UNALIGNED u8*)m_lpBufCur; 
			m_lpBufCur += sizeof(u8); 
		#else
			Read ( &by, sizeof(u8) );
		#endif // _BUFFERED_ARCHIVE_

			return *this;
		}

		archive& archive::operator>>(u16& w)
		{
		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u16) > m_lpBufMax)
				FillBuffer(sizeof(u16) - (u32)(m_lpBufMax - m_lpBufCur));
			w = *(UNALIGNED u16*)m_lpBufCur; 
			m_lpBufCur += sizeof(u16); 
		#else
			Read ( &w, sizeof(u16) );
		#endif // _BUFFERED_ARCHIVE_

			if ( IsByteSwapping() )
				w=lib::u16Swap(w);
			return *this;
		}

		archive& archive::operator>>(u32& dw)
		{
		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u32) > m_lpBufMax)
				FillBuffer(sizeof(u32) - (u32)(m_lpBufMax - m_lpBufCur));
			dw = *(UNALIGNED u32*)m_lpBufCur; 
			m_lpBufCur += sizeof(u32); 
		#else
			Read ( &dw, sizeof(u32) );
		#endif // _BUFFERED_ARCHIVE_

			if ( IsByteSwapping() )
				dw=lib::u32Swap(dw);
			return *this;
		}

		archive& archive::operator>>(u64& ddw)
		{
		#if defined(_BUFFERED_ARCHIVE_) 
			if (m_lpBufCur + sizeof(u64) > m_lpBufMax)
				FillBuffer(sizeof(u64) - (u64)(m_lpBufMax - m_lpBufCur));
			ddw = *(UNALIGNED u64*)m_lpBufCur; 
			m_lpBufCur += sizeof(u64); 
		#else
			Read ( &ddw, sizeof(u64) );
		#endif // _BUFFERED_ARCHIVE_

			if ( IsByteSwapping() )
				ddw=lib::u64Swap(ddw);
			return *this;
		}



		archive& archive::operator>>(long& l)
		{
			_ASSERTE(sizeof(long) == sizeof(u32));
			return operator>>((u32&) l);
		}


		archive& archive::operator>>(f32& f)
		{
			_ASSERTE(sizeof(f32) == sizeof(u32));
			return operator>>((u32&) f);
		}

		archive& archive::operator>>(f64& f)
		{
			_ASSERTE(sizeof(f64) == sizeof(u64));
			return operator>>((u64&) f);
		}

		archive& archive::operator<<( char * string )
		{ 
		u16 size = 0;

			if ( string )
				size = chilli::lib::strlen(string);

			*this << size ;

			if ( size )
				Write ( string, size );

			return *this;
		}

		archive& archive::operator>>( char** string )
		{ 
			SAFEFREE ( *string );

			u16 size = 0 ;
			*this >> size ;

			if ( size ) {
				*string = (char*)malloc(size+1);
				memset(*string,0,size+1);
				Read ( *string, size );
				//string[size] = '\0';
			}

			return *this;
		}

	}
	// namespace lib

}
// namespace tme


