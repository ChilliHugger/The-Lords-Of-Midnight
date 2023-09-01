//
//  cflags.h
//  midnight
//
//  Created by Chris Wild on 03/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef cflags_h
#define cflags_h

#include "mxtypes.h"
#include "carchive.h"

template <class T, typename S>
class  eflags
{
public:
    ~eflags(){}
    MXINLINE void Clear()                            { m_flags = 0; }
    MXINLINE void ShiftLeft()                        { m_flags <<=1; }
    MXINLINE void ShiftRight()                       { m_flags >>=1; }
    archive& Serialize ( archive& ar )
    {
        if ( ar.IsStoring() ) { ar << m_flags ; }else{ar >> m_flags ;}
        return ar;
    }
    MXINLINE eflags()                                { m_flags = 0 ; }
    MXINLINE void Set ( T f )                        { m_flags |= (S)f ; }
    MXINLINE void Set ( T f, bool v )                { if ( v ) Set(f); else Reset(f); }
    MXINLINE void Reset ( T f )                      { m_flags &= ~(S)f ; }
    MXINLINE void Toggle ( T f )                     { if ( Is(f) ) Reset(f); else Set(f); }
    MXINLINE bool Is ( T f) const                    { return m_flags&(S)f ? TRUE : FALSE  ; }
    MXINLINE operator S() const                      { return m_flags; }
    friend MXINLINE archive& operator<<( archive& ar, eflags<T,S>& f )        { return f.Serialize(ar); }
    friend MXINLINE archive& operator>>( archive& ar, eflags<T,S>& f )        { return f.Serialize(ar); }
    
private:
    S   m_flags;
};

// flags
template <typename T>
class  flags
{
public:
    ~flags(){}
    void Clear()                                        { m_flags = 0; }
    void ShiftLeft()                                    { m_flags <<=1; }
    void ShiftRight()                                   { m_flags >>=1; }
    archive& Serialize ( archive& ar )
    {
        if ( ar.IsStoring() ) { ar << m_flags ; }else{ar >> m_flags ;}
        return ar;
    }
    MXINLINE flags(T value)                             { m_flags = value;}
    MXINLINE flags()                                    { m_flags = 0 ; }
    MXINLINE void Set ( T f )                           { m_flags |= f ; }
    MXINLINE void Reset ( T f )                         { m_flags &= ~f ; }
    MXINLINE void Toggle ( T f )                        { if ( Is(f) ) Reset(f); else Set(f); }
    MXINLINE bool Is ( T f) const                       { return m_flags&f ? TRUE : FALSE  ; }
    MXINLINE operator T() const                         { return m_flags; }
    friend MXINLINE archive& operator<<( archive& ar, flags<T>& f )        { return f.Serialize(ar); }
    friend MXINLINE archive& operator>>( archive& ar, flags<T>& f )        { return f.Serialize(ar); }
private:
    T m_flags;
};

typedef flags<u64>  flags64;
typedef flags<u32>  flags32;
typedef flags<u16>  flags16;
typedef flags<u8>   flags8;

#endif /* cflags_h */
