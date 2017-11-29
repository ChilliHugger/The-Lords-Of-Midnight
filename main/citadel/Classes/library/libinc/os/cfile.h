// file.h: interface for the file class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _file_H_INCLUDED_
#define _file_H_INCLUDED_


#include "../mxtypes.h"
#include <stdlib.h>


/////////////////////////////////////////////////////////////////////////////
// File - raw unbuffered disk file I/O

namespace chilli {
	namespace os {

        using namespace chilli::types ;
        
		typedef class file* PFILE ;
		class  file
		{

		public:
		// Flag values
			enum OpenFlags {
				modeRead =          0x0000,
				modeWrite =         0x0001,
				modeReadWrite =     0x0002,
				shareCompat =       0x0000,
				shareExclusive =    0x0010,
				shareDenyWrite =    0x0020,
				shareDenyRead =     0x0030,
				shareDenyNone =     0x0040,
				modeNoInherit =     0x0080,
				modeCreate =        0x1000,
				modeNoTruncate =    0x2000,
				typeText =          0x4000, // typeText and typeBinary are used in
				typeBinary =   (int)0x8000 // derived classes only
				};

			enum Attribute {
				normal =    0x00,
				readOnly =  0x01,
				hidden =    0x02,
				system =    0x04,
				volume =    0x08,
				directory = 0x10,
				archive =   0x20
				};

			enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

			enum { hFileNull = -1 };

		// Constructors
            file();
			file(int hFile);
			file(LPCSTR lpszFileName, u32 nOpenFlags);
			~file();

		// Attributes
			DWORD GetPosition() const;

		// Operations
			bool Open(LPCSTR lpszFileName, u32 nOpenFlags );
			LONG Seek(LONG lOff=0, u32 nFrom=begin) const;
			bool SetLength(DWORD dwNewLen);
			DWORD GetLength() const;
			u32 Read(void* lpBuf, u32 nCount);
			bool Write(const void* lpBuf, u32 nCount);
			void Abort();
			bool Flush();
			void Close();

			bool IsOpen() const ;
			bool IsValidHandle() const ;
			DWORD SeekToEnd();
			void SeekToBegin();
			LPSTR GetFileName();

		protected:
			bool m_bCloseOnDelete;
			char m_strFileName[MAX_PATH];
			int	m_hFile;
		};

#ifdef _UNIX_
#define O_BINARY 0
#endif
        
	}
}

#ifndef _DONT_DEFINE_MFC_CLASSES
namespace chilli {
    namespace os {
        /* os::file */
        MXINLINE file::file()											{ m_hFile = -1; m_bCloseOnDelete = FALSE; }
        MXINLINE file::file(int hFile)								{ m_hFile = hFile; m_bCloseOnDelete = FALSE; }
        MXINLINE file::file(LPCSTR lpszFileName, u32 nOpenFlags)		{ Open(lpszFileName, nOpenFlags); }
        MXINLINE file::~file()										{ if (IsValidHandle() && m_bCloseOnDelete) Close(); }
        MXINLINE LPSTR file::GetFileName()							{ return m_strFileName; }
        MXINLINE DWORD file::SeekToEnd()								{ return Seek(0, end); }
        MXINLINE void file::SeekToBegin()								{ Seek(0, begin); }
        MXINLINE bool file::IsOpen() const							{ return IsValidHandle(); }
        MXINLINE bool file::IsValidHandle() const						{ return m_hFile != -1; }
    }
}
#endif //_DONT_DEFINE_MFC_CLASSES



#endif // _file_H_INCLUDED_
