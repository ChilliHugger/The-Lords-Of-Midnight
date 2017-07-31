#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

#include "file.h"




/*
 * void *FILE_Load( LPSTR szFilename)
 * Allocates memory for the file and reads the entire contents into
 * memory.
 *
 * ENTRY:       LPSTR szFilename - File to get!
 * EXIT:        void *          - pointer to file in memory
 *
 * NOTE:        Remember to free the memory!
 */

void *FILE_Load( LPCSTR szFilename, int& size)
{
FILE *infile;
size_t length;
void *buffer;

	if ( (infile = fopen( szFilename, "rb" )) == NULL )
		return NULL;

	length = FILE_Length ( infile );

	if( (buffer = malloc( length )) == NULL ) {
		fclose( infile );
		return NULL;
	}

	if ( (fread( buffer, sizeof(u8), length, infile )) != length )	{
		fclose( infile );
		free( buffer );
		return NULL;
	}
	
	fclose( infile );

	size=(int)length;

	return ( buffer );
}

void FILE_Unload ( void *mem )
{
	free ( mem );
}

/*
 * size_t FILE_Length ( FILE *infile )
 * Returns the length of the file pointed to by the stream.
 *
 * ENTRY:       FILE * infile   - File stream handle
 * EXIT:        size_t          - length of file
 *
 * NOTE:        This function remembers the current file position, and thus
 *              can be called at any point during file access.
 */

size_t FILE_Length ( FILE *infile )
{
size_t length;
long curpos;

	/* remember current file position */
	curpos = ftell ( infile );

	/* then, move to the end of the file */
	fseek ( infile, 0, SEEK_END );

	/* giving us its length therefore! */
	length = ftell ( infile );

	/* return the file seek position to whence we came */
	fseek ( infile, curpos, SEEK_SET );

	return length;
}
