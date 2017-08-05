// archive.h: interface for the archive class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CARCHIVE_H_INCLUDED_
#define _CARCHIVE_H_INCLUDED_

#include <stdlib.h>

//#if !defined(_UNIX_)
//#define _BUFFERED_ARCHIVE_
//#endif

//#if !defined(_BUFFERED_ARCHIVE_)
//#include <stdio.h>
//#define ARFILE	FILE*
//#else
#define ARFILE	os::file*
//#endif

#define	UNALIGNED 

class file ;


namespace chilli {
	namespace lib {

		class  archive
		{
		public:
		// Flag values
			enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bByteSwap = 4 };

			archive();
			archive( ARFILE pFile, u32 nMode );
			virtual ~archive();

			void Open( ARFILE pFile, u32 nMode );
			ARFILE GetFile() const;

		// Attributes
			BOOL IsLoading() const;
			BOOL IsStoring() const;
			BOOL IsByteSwapping() const;
			BOOL IsBufferEmpty() const;


		// Operations
			virtual u32 Read(void* lpBuf, u32 nMax);
			virtual void Write(const void* lpBuf, u32 nMax);
			virtual void Flush();
			virtual void Close();
			virtual void Abort();

		public:
			// insertion operations

			archive& operator<<(u8 by);
			archive& operator<<(u16 w);
			archive& operator<<(u32 l);
			archive& operator<<(u64 l);
			archive& operator<<(f32 f);
			archive& operator<<(f64 d);
			archive& operator<<(long l);
			archive& operator<<(char * string );
			archive& operator<<(int i);
			archive& operator<<(short w);
			archive& operator<<(char ch);

			// extraction operations
			archive& operator>>(u8& by);
			archive& operator>>(u16& w);
			archive& operator>>(u32& dw);
			archive& operator>>(u64& dw);
			archive& operator>>(f32& f);
			archive& operator>>(f64& d);
			archive& operator>>(long& l);
			archive& operator>>(char ** s );
			archive& operator>>(int& i);
			archive& operator>>(short& w);
			archive& operator>>(char& ch);


		// Implementation
		public:
			void FillBuffer(u32 nBytesNeeded);

		//protected:
			// archive objects cannot be copied or assigned
			archive(const archive& arSrc);
			void operator=(const archive& arSrc);

			BOOL		m_nMode;
			BOOL		m_bUserBuf;
			int			m_nBufSize;
			ARFILE		m_pFile;
			BYTE*		m_lpBufCur;
			BYTE*		m_lpBufMax;
			BYTE*		m_lpBufStart;
			char		m_strFileName[MAX_PATH];

		};
	}
}

#endif // _CARCHIVE_H_INCLUDED_
