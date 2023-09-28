/*
 * FILE:    collections.cpp
 *
 * PROJECT: MidnightEngine
 *
 * CREATED:
 *
 * AUTHOR:  Chris Wild
 *
 * Copyright 2011 Chilli Hugger. All rights reserved.
 *
 * PURPOSE:
 *
 *
 */
#pragma once

#include <algorithm>
#include <string>

namespace tme {
    class mxentity;
    class mxscenario;

    mxentity* CreateEntity(mxscenario* scenario, id_type_t type);
}


using namespace chilli;
using namespace chilli::lib;
using chilli::collections::c_mxid;

namespace tme {
    namespace collections {


// class entities collection
template<typename T>
class entities
{
public:
    using EntityArray = std::vector<T>;

    typename EntityArray::iterator begin() { return m_elements.begin(); }
    typename EntityArray::iterator end() { return m_elements.end(); }


public:
    entities(void);
    ~entities(void);
    
    entities(const entities<T>& list);
    entities<T> Clone() const;
    
    bool Create( u32 count );
    bool Create( mxscenario* scenario, id_type_t type, u32 count );
    void Clear();
    bool CreateIdtCollection ( c_mxid& obj );
    u32 Count() const;

    //void operator = ( const entities<T>& src );
    T& operator[]( u32 nSubscript );
    void operator += ( T o );

    void Destroy( void );
    virtual void Serialize( chilli::lib::archive& ar );
    T FindSymbol ( const std::string& name ) ;
    void Sort ( int hint ) ; // not thread safe

    bool Add ( T );
    u32 Compact( void );
    void Resize ( u32 newsize );
    s32 IndexOf ( T entity );
    
    T First();

protected:
    u32         m_max;
    u32         m_used;
    EntityArray m_elements;
    bool        m_bOwner;
};
// class entities collection

// class infos collection
template<typename T>
class infos : public entities<T>
{
public:
    infos(void);
    ~infos(void);
    virtual void Serialize( chilli::lib::archive& ar ) override;
};
// class infos collection

template <typename T>
entities<T>::entities(void)
{
    Clear();
    m_bOwner=false;
}

template <typename T>
entities<T>::~entities(void)
{
    Destroy();
}

template <typename T>
void entities<T>::Clear( void )
{
    m_elements.clear();
    m_used=0;
    m_max=0;
}

template <typename T>
entities<T>::entities(const entities<T>& list)
{
    Clear();
    Create(list.Count());
    for ( u32 ii=0; ii<list.Count(); ii++ ) {
        m_elements[ii] = list.m_elements[ii];
        m_used++;
    }
    m_bOwner = false;
}
 
template <typename T>
entities<T> entities<T>::Clone() const
{
    entities<T> value(*this);

    //const auto value = new entities<T>(*this);
    return value;
}


//template <typename T>
//void entities<T>::operator = ( const entities& src )
//{
//    Create(src.Count());
//    for ( u32 ii=0; ii<src.Count(); ii++ ) {
//        m_elements[ii] = src.m_elements[ii];
//        m_used++;
//    }
//    m_bOwner=false;
//}

template <typename T>
bool entities<T>::CreateIdtCollection ( c_mxid& obj )
{
    obj.Create( Count() );
    for ( u32 ii=0; ii<Count(); ii++ ) {
        auto entity = m_elements[ii];
        obj[ii] = entity != nullptr
            ? MAKE_ID(entity->Type(),entity->Id())
            : IDT_NONE;
    }

    return true;
}

template <typename T>
bool entities<T>::Create( u32 count )
{
    Destroy();
    m_max = count;
    m_used=0;
    m_elements.resize(m_max);
    return true ;
}

template <typename T>
bool entities<T>::Create( mxscenario* scenario, id_type_t type, u32 count )
{
    Create(count);

    for ( u32 ii=0; ii<count; ii++, m_used++ )
        m_elements[ii] = static_cast<T>(CreateEntity(scenario,type));

    m_bOwner=true;
    return true ;
}

template <typename T>
void entities<T>::Destroy( )
{
    if ( m_bOwner ) {
        for(int ii=0; ii<Count(); ii++)
            SAFEDELETE ( m_elements[ii] );
    }

    Clear();
    m_bOwner = false;
}

template <typename T>
T entities<T>::FindSymbol ( const std::string& name )
{
    for ( u32 ii=0; ii<Count(); ii++ ) {
        if ( name == m_elements[ii]->Symbol() )
            return m_elements[ii] ;
    }
    return nullptr ;
}

template <typename T>
s32 entities<T>::IndexOf ( T entity )
{
    for ( u32 ii=0; ii<Count(); ii++ ) {
        if ( m_elements[ii] == entity )
            return ii ;
    }
    return -1 ;
}

template <typename T>
u32 entities<T>::Count() const
{
    return m_used;
}

template <typename T>
u32 entities<T>::Compact( void )
{
    Resize(Count());
    return Count();
}

template <typename T>
void entities<T>::Resize ( u32 newsize )
{
    if ( newsize == m_max )
        return ;

    if ( newsize == 0 ) {
        // lose old array
        m_elements.clear();
    }

    m_elements.resize(newsize);
    m_max = newsize;
    m_used = std::min<int>( m_used, m_max );

}

template <typename T>
void entities<T>::operator += ( T o )
{
    Add(o);
}

template <typename T>
bool entities<T>::Add ( T o )
{
    if ( m_bOwner ) return false;

    if ( m_used == m_max ) {
        m_elements.push_back(o);
        m_used++;
        m_max++;
        return true;
    }
    
    m_elements[m_used++] = o;
    return true;
}

template <typename T>
T& entities<T>::operator[]( u32 nSubscript )
{
static T iErr = nullptr;

    if ( /*nSubscript>=0 &&*/ nSubscript<Count() )
        return m_elements[ nSubscript ];
    return iErr;
}

template <typename T>
void entities<T>::Serialize( archive& ar )
{
u32 ii;
int id;

    if ( ar.IsStoring() ) {
        for ( ii=0; ii<Count(); ii++ ) {
            id = ii+1;
            ar << id;
            m_elements[ii]->Serialize ( ar );
        }
    }else{
        for ( ii=0; ii<Count(); ii++ ) {
            ar >> id;
            id--;
            m_elements[id]->Serialize ( ar );
            m_elements[id]->Id(id+1);
        }
    }
}

template <typename T>
T entities<T>::First()
{
    return ( Count()>0 )
        ? m_elements.front()
        : nullptr;
}

template <typename T>
void entities<T>::Sort ( int hint )
{
    int sort_hint = hint;

    std::sort(begin(), end(), [sort_hint](const T lhs, const T rhs)
    {
        return lhs->Compare(rhs,sort_hint);
    });
}

template <typename T>
void infos<T>::Serialize( archive& ar )
{
u32 ii;
int id;
    if ( ar.IsStoring() ) {
        for ( ii=0; ii<entities<T>::Count(); ii++ ) {
            ar << ii;
            entities<T>::m_elements[ii]->Serialize ( ar );
        }
    }else{
        for ( ii=0; ii<entities<T>::Count(); ii++ ) {
            ar >> id;
            entities<T>::m_elements[id]->Serialize ( ar );
            entities<T>::m_elements[id]->Id(id);
        }
    }
}

template <typename T>
infos<T>::infos(void)
{
}

template <typename T>
infos<T>::~infos(void)
{
}

}
}
