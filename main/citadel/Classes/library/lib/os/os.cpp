/*
 * FILE:    os.cpp
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
#include <stdlib.h>
#include <string.h>

namespace chilli {
	
	namespace os {
        
        void SetCurrentDirectory ( LPCSTR dir )
        {
        }
        
        LPSTR GetCurrentDirectory ( u32 max, LPSTR dir )
        {
            return (LPSTR)"" ;
        }
        
        char* Fullpath ( char *UserBuf, const char *path, u32 maxlen )
        {
            char    *buf;
            size_t   cbPath;
            
            if( !path || !*path )
                return( (char*)GetCurrentDirectory( maxlen, UserBuf ) );
            
            if (UserBuf && (strlen(path) > maxlen))
                return NULL;
            
            // allocate buffer if necessary
            
            if( !UserBuf )
            {
                if( !(buf = (char*)malloc(MAX_PATH)) )
                {
                    //                 errno = ENOMEM;
                    return( NULL );
                }
                else
                {
                    maxlen = MAX_PATH;
                }
            }
            else
            {
                buf = UserBuf;
            }
            
            cbPath = strlen(path);
            if (cbPath > maxlen)
            {
                if (!UserBuf)
                {
                    free(buf);
                }
                //            errno = ERANGE;
                return NULL;
            }
            
            maxlen = maxlen - (u32)cbPath;
            if (*path == ':')
            {
                maxlen++;        //this ":" is not counted
            }
            
            if ((*path == ':') ||
                (strstr(path, ":") == NULL) )
            {
                //partial path
                if (GetCurrentDirectory( maxlen,buf ) != NULL)
                {
                    if (*path == ':')
                    {
                        lib::c_strcat(buf, path+1);
                    }
                    else
                    {
                        lib::c_strcat(buf, path);
                    }
                }
                else
                {
                    if (!UserBuf)
                    {
                        free(buf);
                    }
                    return( NULL);
                }
            }
            else
            {
                //fullpath
                lib::c_strcpy(buf, path);
            }
            
            return buf;
            
        }
        
        
        
	}
	// namespace os
}
// namespace tme


