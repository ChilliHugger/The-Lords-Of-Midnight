//
//  cflags.h
//  midnight
//
//  Created by Chris Wild on 03/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef cflags_h
#define cflags_h

#include "../../libinc/mxtypes.h"
#include "../../libinc/sw/carchive.h"

template <class T>
class  flags
{
public:
    ~flags(){}
    MXINLINE void Clear()                            { m_flags = 0; }
    MXINLINE void ShiftLeft()                        { m_flags <<=1; }
    MXINLINE void ShiftRight()                       { m_flags >>=1; }
    archive& Serialize ( archive& ar )
    {
        if ( ar.IsStoring() ) { ar << m_flags ; }else{ar >> m_flags ;}
        return ar;
    }
    
    MXINLINE flags()                                 { m_flags = 0 ; }
    MXINLINE void Set ( T f )                        { m_flags |= (u32)f ; }
    MXINLINE void Reset ( T f )                      { m_flags &= ~(u32)f ; }
    MXINLINE void Toggle ( T f )                     { if ( Is(f) ) Reset(f); else Set(f); }
    MXINLINE bool Is ( T f) const                    { return m_flags&(u32)f ? TRUE : FALSE  ; }
    MXINLINE operator u32() const                    { return m_flags; }
    friend MXINLINE archive& operator<<( archive& ar, flags& f )        { return f.Serialize(ar); }
    friend MXINLINE archive& operator>>( archive& ar, flags& f )        { return f.Serialize(ar); }
    
private:
    u32        m_flags;
};


#endif /* cflags_h */
