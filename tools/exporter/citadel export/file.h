#ifndef _SNAPSHOTS_H_INCLUDED_
#define _SNAPSHOTS_H_INCLUDED_

#include <stdio.h>
#include "types.h"


u8 *Openfile ( LPCSTR szFilename );
size_t FILE_Length ( FILE *infile );
void FILE_Unload ( void *mem );
void *FILE_Load( LPCSTR szFilename, int& size);

#endif //_SNAPSHOTS_H_INCLUDED_
