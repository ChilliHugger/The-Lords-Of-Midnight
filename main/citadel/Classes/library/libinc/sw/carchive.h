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

namespace chilli
{
    namespace os {
        class file;
    }
}

#define ARFILE	chilli::os::file*
//#endif

#define	UNALIGNED 

class file ;


namespace chilli {
	namespace lib {

        using namespace chilli::types;
        
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
			bool IsLoading() const;
			bool IsStoring() const;
			bool IsByteSwapping() const;
			bool IsBufferEmpty() const;


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
			archive& operator<<(char* s );
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

			bool		m_nMode;
			bool		m_bUserBuf;
			int			m_nBufSize;
			ARFILE		m_pFile;
			BYTE*		m_lpBufCur;
			BYTE*		m_lpBufMax;
			BYTE*		m_lpBufStart;
			char		m_strFileName[MAX_PATH];

		};
	}
}

namespace chilli {
    namespace lib {
        
        /* archive */
        MXINLINE bool archive::IsLoading() const						{ return (m_nMode & archive::load) != 0; }
        MXINLINE bool archive::IsStoring() const						{ return (m_nMode & archive::load) == 0; }
        MXINLINE bool archive::IsByteSwapping() const					{ return (m_nMode & archive::bByteSwap); }
        MXINLINE bool archive::IsBufferEmpty() const					{ return m_lpBufCur == m_lpBufMax; }
        MXINLINE ARFILE archive::GetFile() const					{ return m_pFile; }
        MXINLINE archive& archive::operator<<(int i)					{ return archive::operator<<((u32)i); }
        MXINLINE archive& archive::operator<<(short w)				{ return archive::operator<<((u16)w); }
        MXINLINE archive& archive::operator<<(char ch)				{ return archive::operator<<((BYTE)ch); }
        MXINLINE archive& archive::operator>>(int& i)					{ return archive::operator>>((u32&)i); }
        MXINLINE archive& archive::operator>>(short& w)				{ return archive::operator>>((u16&)w); }
        MXINLINE archive& archive::operator>>(char& ch)				{ return archive::operator>>((u8&)ch); }
        
        // archive output helpers
        MXINLINE archive& operator<<(archive& ar, const size& s)		{ ar.Write(&s, sizeof(size)); return ar; }
        MXINLINE archive& operator<<(archive& ar, const point& p)		{ ar.Write(&p, sizeof(point)); return ar; }
        MXINLINE archive& operator<<(archive& ar, const rect& r)		{ ar.Write(&r, sizeof(rect)); return ar; }
        MXINLINE archive& operator>>(archive& ar, size& s)             { ar.Read(&s, sizeof(size)); return ar; }
        MXINLINE archive& operator>>(archive& ar, point& p)			{ ar.Read(&p, sizeof(point)); return ar; }
        MXINLINE archive& operator>>(archive& ar, rect& r)             { ar.Read(&r, sizeof(rect)); return ar; }
        MXINLINE archive& operator<<(archive& ar, const range& r)		{ ar.Write(&r, sizeof(range)); return ar; }
        MXINLINE archive& operator>>(archive& ar, range& r)			{ ar.Read(&r, sizeof(range)); return ar; }
    }
}

#endif // _CARCHIVE_H_INCLUDED_
