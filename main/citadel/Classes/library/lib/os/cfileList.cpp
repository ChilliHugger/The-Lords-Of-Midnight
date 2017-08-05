

#include "../../libinc/library.h"

#include <stdio.h>
#ifdef _UNIX_
#include <dirent.h>
#include "string.h"
#else
#include <io.h>
#endif

namespace chilli {

	namespace os {


fileentry::fileentry()
{
	//m_filename=NULL;
}

fileentry::~fileentry()
{
	//SAFEFREE ( m_filename );
}
	
		
filelist::filelist()
{
	m_count=0;
	m_dir=NULL;
	m_filespec=NULL;
	m_attribs=0;

}

filelist::filelist( string filespec, u32 attribs )
{
	m_count=0;
	m_dir=NULL;
	m_filespec="";
	m_attribs=0;
	CreateDir ( filespec, attribs );
}

filelist::~filelist()
{
	Destroy();
}


BOOL filelist::CreateDir ( string filespec, u32 attribs )
{
	Destroy();

//	SAFEFREE ( m_filespec );
	m_filespec = filespec;
	m_attribs=attribs;

	
	if ( CountDirFiles() ) {
		m_dir = new fileentry[m_count];
		if ( m_dir == NULL ) {
			m_count=0;
			return FALSE;
		}

		MakeDirFileList ();
	}

	return TRUE ;
}

BOOL filelist::Destroy()
{
	if ( m_dir ) {
		SAFEDELETEARRAY( m_dir );
//		m_dir=NULL;
	}

	//SAFEFREE ( m_filespec );
	m_filespec = "" ;
	m_attribs=0;
	return TRUE ;
}

void filelist::MakeDirFileList ( void )
{
#ifdef _UNIX_

	DIR *dir;
	struct dirent *dir_entry;
	BOOL found_file;
	unsigned count = 0;
	char drive_letter[MAX_PATH];
	char directory_name[MAX_PATH];
	char file_name[MAX_PATH], dir_file_name[MAX_PATH];
	char file_extension[MAX_PATH], dir_file_extension[MAX_PATH];

	lib::splitpath(m_filespec, drive_letter, directory_name, file_name, file_extension);

	if((dir = opendir( directory_name )) != NULL)
	{
		while( (dir_entry = readdir( dir )) )
		{
			memset( dir_file_name, 0, MAX_PATH );
			memset( dir_file_extension, 0, MAX_PATH );

			found_file = FALSE;

			lib::splitpath(dir_entry->d_name, NULL, NULL, dir_file_name, dir_file_extension);
			
			// If filename & extension match
			// or filename is wildcard & extension match
			// or filename matches & extension is wildcard
			// or filename & extension are both wildcards
			// or filenames match and there is no extension...

			if( ( !lib::stricmp( file_name, dir_file_name ) && !lib::stricmp( file_extension, dir_file_extension ) ) ||
				( !lib::stricmp( file_name, "*" ) && !lib::stricmp( file_extension, dir_file_extension ) ) ||
				( !lib::stricmp( file_name, dir_file_name ) && !lib::stricmp( file_extension, ".*" ) ) ||
				( !lib::stricmp( file_name, "*" ) && !lib::stricmp( file_extension, ".*" ) ) ||
				( !lib::stricmp( file_name, dir_file_name ) && !lib::stricmp( file_extension, "" ) && !lib::stricmp( dir_file_extension, "" ) ) )
			{
				found_file = TRUE;
			}

			if(found_file)
			{
				lib::strcat(dir_file_name, dir_file_extension);
				
				m_dir[count++].m_filename = dir_file_name ;
			}
		}
	
		closedir( dir );
	}
#else
	_finddata_t		FindFileData;
	int			hFileFind;
	int			count=0;

	if ( (hFileFind = _findfirst( m_filespec, &FindFileData )) != -1 )
	{
		do
		{
			if ( count==m_count )
				break;
			if ( (FindFileData.attrib & m_attribs)!=m_attribs )
				continue;
			m_dir[count++].m_filename = FindFileData.name;

		} while ( _findnext ( hFileFind, &FindFileData ) == 0 );
		_findclose ( hFileFind );
	}
#endif
}


u32 filelist::CountDirFiles ()
{
#ifdef _UNIX_

	DIR *dir;
	struct dirent *dir_entry;
	unsigned count = 0;
	char drive_letter[MAX_PATH];
	char directory_name[MAX_PATH];
	char file_name[MAX_PATH], dir_file_name[MAX_PATH];
	char file_extension[MAX_PATH], dir_file_extension[MAX_PATH];

	lib::splitpath(m_filespec, drive_letter, directory_name, file_name, file_extension);

	if((dir = opendir( directory_name )) != NULL)
	{
		while( (dir_entry = readdir( dir )) )
		{
			memset( dir_file_name, 0, MAX_PATH );
			memset( dir_file_extension, 0, MAX_PATH );

			lib::splitpath( dir_entry->d_name, NULL, NULL, dir_file_name, dir_file_extension );

			// If filename & extension match
			// or filename is wildcard & extension match
			// or filename matches & extension is wildcard
			// or filename & extension are both wildcards
			// or filenames match and there is no extension...

			if( ( !lib::stricmp( file_name, dir_file_name ) && !lib::stricmp( file_extension, dir_file_extension ) ) ||
				( !lib::stricmp( file_name, "*" ) && !lib::stricmp( file_extension, dir_file_extension ) ) ||
				( !lib::stricmp( file_name, dir_file_name ) && !lib::stricmp( file_extension, ".*" ) ) ||
				( !lib::stricmp( file_name, "*" ) && !lib::stricmp( file_extension, ".*" ) ) ||
				( !lib::stricmp( file_name, dir_file_name ) && !lib::stricmp( file_extension, "" ) && !lib::stricmp( dir_file_extension, "" ) ) )
			{
				count++;
			}
		}
	
		closedir( dir );
	}
#else
	_finddata_t		FindFileData;
	int			hFileFind;
	int			count = 0;
	
	if ( (hFileFind = _findfirst( m_filespec, &FindFileData )) != -1 )
	{
		do
		{
			if ( (FindFileData.attrib & m_attribs)==m_attribs )
				count++;
		} while ( _findnext ( hFileFind, &FindFileData ) == 0 );
		_findclose ( hFileFind );
	}
#endif
	m_count = count;

	return m_count;
}

fileentry& filelist::GetAt ( u32 index ) const
{
	_ASSERTE ( index>=0 && index<m_count );
	return m_dir[index];
}


static int file_compare( const void *arg1, const void *arg2 )
{
	/* Compare all of both strings: */
	fileentry* f1 = (fileentry*)arg1;
	fileentry* f2 = (fileentry*)arg2;
	return lib::stricmp( f1->m_filename, f2->m_filename );
}

void filelist::Sort ( void )
{
	/* Sort remaining args using Quicksort algorithm: */
	qsort( (void *)m_dir, (size_t)m_count, sizeof( fileentry ), file_compare );

}



} 
// namespace os

}
// namespace tme
