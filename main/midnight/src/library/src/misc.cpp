#include "../inc/mxtypes.h"
#include "../inc/collections.h"
#include "../inc/misc.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <sstream>
#include <iostream>
#include <vector>
#include <string>


namespace chilli {

    namespace lib {

        using namespace chilli::collections;

        void JumbleArray ( int* array, int max )
        {
            // jumble them up
            for ( int ii=0; ii<max; ii++ ) {
                int a=ii; //mxrandom(1,max)-1;
                int b= randomno::instance.get(1,max)-1;
                int temp;
                temp = array[a];
                array[a]=array[b];
                array[b]=temp;
            }
        }

        int ConcatArray( int* array, int max )
        {
        int i;
        int count=0;

            _MXASSERTE( array != NULL );
            bool Done = FALSE;
            while( !Done )
            {
                Done = TRUE;
                for(  i = 0; i < max-1; i++ ) {
                    if( (array[i] == 0) && array[i+1] ) {
                        array[i] = array[i+1];
                        array[i+1] = 0;
                        Done = FALSE;
                    };
                };
            };

            for ( i=0, count=0; i<max; i++, count++ ) {
                if ( array[i] == 0 )
                    break;

            }

            return count;
        };

        int GetToken ( LPCSTR token, token_t array[], int max )
        {
            if ( token ) {
                for ( int ii=0; ii<max; ii++ ) {
                    if (c_stricmp( array[ii].token, token ) == 0 )
                        return array[ii].value;
                }
            }
            return 0 ;
        }

        LPSTR c_strdup ( LPCSTR s )
        {
            char *memory;
            if (!s)
                    return(NULL);
            if ((memory = (char*)malloc(c_strlen(s) + 1)))
                    return(c_strcpy(memory,s));
            return(NULL);
        }

        int c_stricmp(LPCSTR dst, LPCSTR src)
        {
            if ( dst == NULL && src == NULL ) return 0;
            if ( dst == NULL ) return -1;
            if ( src == NULL ) return 1 ;
            int f,l;
            do {
                if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
                    f -= ('A' - 'a');

                if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
                    l -= ('A' - 'a');
            } while ( f && (f == l) );
            return(f - l);
        }

        int c_strnicmp ( LPCSTR first, LPCSTR last, size_t count )
        {
            if ( first == NULL && last == NULL ) return 0;
            if ( first == NULL ) return -1;
            if ( last == NULL ) return 1 ;
            int f,l;
            if ( count )
            {
                    do {
                        if ( ((f = (unsigned char)(*(first++))) >= 'A') &&
                            (f <= 'Z') )
                            f -= 'A' - 'a';
                        if ( ((l = (unsigned char)(*(last++))) >= 'A') &&
                            (l <= 'Z') )
                            l -= 'A' - 'a';
                    } while ( --count && f && (f == l) );
                return( f - l );
            }
            return( 0 );
        }

        char toupper ( char c )
        {
            if ( c>='a' && c<='z' )
                c = mkupper(c);
            //else if ( c>='�' && c<='�' )
            //    c = mkupper(c);
            return c;
        }

        char tolower ( char c )
        {
            if ( c>='A' &&  c<='Z' )
                c=mklower(c);
            //else if ( c>='�' &&  c<='�' )
            //    c=mklower(c);
            return c;
        }

        LPSTR c_strupr ( LPSTR text )
        {
        LPSTR output=text;
        char c;
            if ( text == NULL ) return NULL ;
            while ( *text != '\0' ) {
                c = *text;
                if ( c>='a' && c<='z' )
                    *text = mkupper(c);
                //else if ( c>='�' && c<='�' )
                //    *text = mkupper(c);
                text++;
            }
            return output;
        }

        LPSTR c_strlwr ( LPSTR text )
        {
        LPSTR output=text;
        char c;
            while ( *text != '\0' ) {
                c = *text;
                if ( c>='A' &&  c<='Z' )
                    *text=mklower(c);
                //else if ( c>='�' &&  c<='�' )
                //    *text=mklower(c);
                text++;
            }
            return output;
        }

        LPSTR c_strcat ( LPSTR dst, LPCSTR src )
        {
            if ( src == NULL ) return dst;
                LPSTR cp = dst;
                while( *cp ) cp++;
                while( (*cp++ = *src++) ) ;
                return( dst );

        }

        size_t c_strlen ( LPCSTR str )
        {
                if ( str==NULL ) return 0;
                LPCSTR eos = str;
                while( *eos++ ) ;
                return( (int)(eos - str - 1) );
        }

        LPSTR c_strcpy ( LPSTR dst, LPCSTR src )
        {
            _MXASSERTE ( dst != NULL );
            if ( dst == NULL )
                return NULL ;

            if ( src == NULL ) {
                dst[0] = '\0';
                return dst;
            }

            LPSTR cp = dst;
            while( (*cp++ = *src++) );
            return( dst );
        }

        u16   u16Swap (u16 l)
        {
        u8    b1,b2;
            b1 = l&255;
            b2 = (l>>8)&255;
            return (b1<<8) + b2;
        }

        u32    u32Swap (u32 l)
        {
        u8    b1,b2,b3,b4;
            b1 = l&255;
            b2 = (l>>8)&255;
            b3 = (l>>16)&255;
            b4 = (l>>24)&255;
            return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
        }

        u64        u64Swap (u64 l)
        {
        u64        temp;
        u8*        dst = (u8*)&temp;
        u8*        src = (u8*)&l;
            dst[0]=src[7];
            dst[1]=src[6];
            dst[2]=src[5];
            dst[3]=src[4];
            dst[4]=src[3];
            dst[5]=src[2];
            dst[6]=src[1];
            dst[7]=src[0];
            return temp;
        }

        f32    f32Swap (f32 l)
        {
        f32        temp;
        u8*        dst = (u8*)&temp;
        u8*        src = (u8*)&l;
            dst[0]=src[3];
            dst[1]=src[2];
            dst[2]=src[1];
            dst[3]=src[0];
            return temp;
        }

        f64        f64Swap (f64 l)
        {
        f64        temp;
        u8*        dst = (u8*)&temp;
        u8*        src = (u8*)&l;
            dst[0]=src[7];
            dst[1]=src[6];
            dst[2]=src[5];
            dst[3]=src[4];
            dst[4]=src[3];
            dst[5]=src[2];
            dst[6]=src[1];
            dst[7]=src[0];
            return temp;
        }

        void splitpath( const char *path, char *drive, char *dir, char *fname, char *ext )
        {
        #ifdef _UNIX_

            const char    *start = path,
                *end = start + ( c_strlen( path ) ),
                
                *dir_start = NULL,
                *dir_end = NULL,
                *fname_start = start,
                *fname_end = end,
                *ext_start = NULL,
                *ext_end = NULL,

                *dot = strrchr( path, '.' ),
                *slash = strrchr( path, '/' );

            if(drive)
                c_strcpy(drive, "");

            if(dir)
                c_strcpy(dir, "");

            if(fname)
                c_strcpy(fname, "");

            if(ext)
                c_strcpy(ext, "");

            // Do we have a '/' ?
            if( slash ) {
                dir_start = start;
                dir_end = slash;
                fname_start = start == end ? slash : slash + 1;
            }

            // Do we have a '.' ?
            if( dot ) {
                fname_end = dot;
                ext_start = dot;
                ext_end = end;
            }

            // Copy the directory name, filename, & file extension strings
            if( dir_start && dir_end ) {
                if ( dir ) {
                    memcpy ( dir, dir_start, ( dir_end - dir_start) );
                    dir[dir_end - dir_start] = '/';
                    dir[dir_end - dir_start + 1] = '\0';
                }
            }

            if( fname_start && fname_end ) {
                if ( fname  ) {
                    memcpy ( fname, fname_start, ( fname_end - fname_start) );
                    fname[fname_end - fname_start] = '\0';
                }
            }

            if( ext_start && ext_end ) {
                if ( ext  ) {
                    memcpy( ext, ext_start, ( ext_end - ext_start) );
                    ext[ext_end - ext_start] = '\0';
                }
            }

        #else
            _splitpath( path, drive, dir, fname, ext );
        #endif
        }

        void makepath( char *path, const char *drive, const char *dir, const char *fname, const char *ext )
        {
        #ifdef _UNIX_
        const char* p;
                /* copy drive */
                if (drive && *drive) {
                    *path++ = *drive;
                    *path++ = ':';
                }

                /* copy dir */
                if ((p = dir) && *p) {
                    do {
                        *path++ = *p++;
                    }
                    while (*p);
                    if (*(p-1) != '/' && *(p-1) != '\\') {
                        *path++ = '\\';
                    }
                }

                /* copy fname */
                if ((p = fname)) {
                    while (*p) {
                        *path++ = *p++;
                    }
                }

                /* copy ext, including 0-terminator - check to see if a '.' needs
                * to be inserted.
                */
                if ((p = ext)) {
                    if (*p && *p != '.') {
                            *path++ = '.';
                    }
                    while ((*path++ = *p++))                      ;
                } else {
                    /* better add the 0-terminator */
                    *path = '\0';
                }

        #else
            _makepath( path, drive, dir, fname, ext );
        #endif
        }

        int wildicmp(LPCSTR wild, LPCSTR string)
        {
            //case in-sensitive wildcard compare
            int result;
            LPSTR aw = new char[c_strlen(wild) + 1];
            strcpy(aw, wild);
            LPSTR as = new char[c_strlen(string) + 1];
            strcpy(as, string);

            c_strupr(aw);
            c_strupr(as);

            result = wildcmp(aw, as);

            delete [] as;
            delete [] aw;

            return result;
        }

        int wildcmp(LPCSTR wild, LPCSTR string)
        {
            LPCSTR cp=NULL;
            LPCSTR mp=NULL;

            while ((*string) && (*wild != '*'))
            {
                if ((*wild != *string) && (*wild != '?'))
                    return 0;

                wild++;
                string++;
            }

            while (*string)
            {
                if (*wild == '*')
                {
                    if (!*++wild)
                        return 1;
                    mp = wild;
                    cp = string + 1;
                }
                else if ((*wild == *string) || (*wild == '?'))
                {
                    wild++;
                    string++;
                }
                else
                {
                    wild = mp;
                    string = cp++;
                }
            }

            while (*wild == '*')
                wild++;

            return !*wild;
        }


        u32 BSub(u32 value, u32 amount, u32 min)
        {
        s64 result = (s64)value-amount;
             return (s32) std::max<int>(result,(s64)min);
        }

        u32 BAdd(u32 value, u32 amount, u32 max)
        {
        u64 result = (u64)value+amount;
            return (s32) std::min<int>( result, (u64)max );

        }

        long atol( LPCSTR text )
        {
            if ( text == NULL ) return 0;
            return ::atol(text);
        }
        
    }
    // namespace lib

}
// namespace tme
