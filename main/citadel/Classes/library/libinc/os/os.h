#ifndef _OS_H_INCLUDED_
#define _OS_H_INCLUDED_

#include "../mxtypes.h"


namespace chilli {

	namespace os {

#define	FILEIO_BUFFER_EXCEEDED		(u32)SETERROR( ERR_WARNING, 1, 1)

#ifdef WIN32
	typedef void*		HLIBRARY;
	typedef int ( __stdcall *FUNCADDRESS)();
#endif

		void Sleep ( int mili );
		void MakeRegionPresent(volatile u8 *pb, u32 cb);
		void SystemInfoCheck ( void );
		void SetCurrentDirectory ( LPCSTR dir );
		LPSTR GetCurrentDirectory ( u32 max, LPSTR dir );
		LPSTR Fullpath ( LPSTR UserBuf, LPCSTR path, u32 maxlen );

		#ifdef WIN32
			HLIBRARY LoadLibrary ( LPCSTR filename );
			void UnloadLibrary ( HLIBRARY hLib );
			FUNCADDRESS GetFunctionAddress ( HLIBRARY hLib, LPCSTR functionname );
		#endif

		// class filemanager
		class  filemanager
		{
		public:
			static BOOL Exists ( LPCSTR lpFileName ) ;
			static BOOL ExistsDir ( LPCSTR lpFileName ) ;
			static BOOL Get ( LPCSTR filename, void* lpBuffer, u32 maxSize) ;
			static void* Load ( LPCSTR filename, u32* size=NULL ) ;
			static u32 Size ( LPCSTR filename ) ;
			static void Unload ( u8** ptr ) ;
			static BOOL Save( LPCSTR filename, const void* lpBuffer, u32 size ) ;
			static BOOL Remove(LPCSTR lpszFileName) ;
			static BOOL Rename(LPCSTR lpszOldName, LPCSTR lpszNewName) ;

			static BOOL CreateDirectory(LPCSTR path);
            static BOOL DestroyDirectory ( LPCSTR path );
            
			static LPSTR GetCurrentDirectory ( int max, LPSTR dir );
			static void SetCurrentDirectory ( LPCSTR dir );
            
            static BOOL Copy ( LPCSTR lpszSrcName, LPCSTR lpszDstName );

            static s64 DateTime(LPCSTR lpFileName);
    

		};
		// class filemanager



		// class fileentry
		class  fileentry
		{
		public:
			fileentry();
			virtual ~fileentry();

		public:
			string m_filename;
		};
		// class fileentry

		// class filelist
		class  filelist
		{
		public:

			enum { // see io.h
				NORMAL	=0x00, /* _A_NORMAL    Normal file - No read/write restrictions */
				RDONLY	=0x01, /* _A_RDONLY    Read only file */
				HIDDEN	=0x02, /* _A_HIDDEN    Hidden file */
				SYSTEM	=0x04, /* _A_SYSTEM    System file */
				SUBDIR	=0x10, /* _A_SUBDIR    Subdirectory */
				ARCH	=0x20, /* _A_ARCH	   Archive file */
			};

			filelist();
			filelist( string filespec, u32 attribs=NORMAL );
			virtual ~filelist();

			BOOL CreateDir ( string filespec, u32 attribs=NORMAL );
			BOOL Destroy();

			int Count() const	{ return m_count; };
			void Sort ( void );

			fileentry& GetAt ( u32 index ) const ;
			fileentry& operator[] ( u32 index ) const	{ return GetAt(index); }

		protected:
			void MakeDirFileList ( void );
			u32 CountDirFiles ();

		protected:
			string		m_filespec;
			u32			m_count;
			fileentry*	m_dir;
			u32			m_attribs;

		};
		// filelist

	}
}

#endif // _OS_H_INCLUDED_
