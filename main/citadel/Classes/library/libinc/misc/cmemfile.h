#ifndef _CMEMFILE_H_INCLUDED_
#define _CMEMFILE_H_INCLUDED_

#ifndef _DONT_DEFINE_MFC_CLASSES
  

using namespace chilli::types ;

namespace chilli {
	namespace os {

		class CMemFile : public os::file
		{

		public:
		// Constructors
			/* explicit */ CMemFile(u32 nGrowBytes = 1024);
			CMemFile(BYTE* lpBuffer, u32 nBufferSize, u32 nGrowBytes = 0);

		// Operations
			void Attach(BYTE* lpBuffer, u32 nBufferSize, u32 nGrowBytes = 0);
			BYTE* Detach();

		// Advanced Overridables
		protected:
			virtual BYTE* Alloc(u32 nBytes);
			virtual BYTE* Realloc(BYTE* lpMem, u32 nBytes);
			virtual BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource, u32 nBytes);
			virtual void Free(BYTE* lpMem);
			virtual void GrowFile(u32 dwNewLen);

		// Implementation
		protected:
			u32 m_nGrowBytes;
			u32 m_nPosition;
			u32 m_nBufferSize;
			u32 m_nFileSize;
			BYTE* m_lpBuffer;
			BOOL m_bAutoDelete;

		public:
			virtual ~CMemFile();
			virtual u32 GetPosition() const;
			virtual u32 Seek(u32 lOff=0, u32 nFrom=begin);
			virtual BOOL SetLength(u32 dwNewLen);
			virtual u32 GetLength() const;
			virtual u32 Read(void* lpBuf, u32 nCount);
			virtual BOOL Write(const void* lpBuf, u32 nCount);
			virtual void Abort();
			virtual BOOL Flush();
			virtual void Close();
			virtual BOOL IsOpen() const ;
			virtual u32 ReadLine(void* lpBuf,u32 max);
			virtual BOOL Eof() const;

		};
	}
}
#endif //_DONT_DEFINE_MFC_CLASSES

#endif //_CMEMFILE_H_INCLUDED_
