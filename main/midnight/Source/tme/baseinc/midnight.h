#ifndef _MIDNIGHT_H_INCLUDED_
#define _MIDNIGHT_H_INCLUDED_


#include "../../library/inc/library.h"
#include "lomxtypes.h"

#define VALIDATE_INFO_BLOCK(ptr,t,s) \
    _MXASSERTE ( IsValidAddress( ptr, sizeof( s ), TRUE ) ); \
    if ( ptr == NULL ) \
        return MX_NULL_STRUCT_PTR ; \
    if ( (u32)ID_TYPE(ptr->id) != (u32)t ) \
        return MX_INVALID_INFO_TYPE; \
    if ( (u32)ptr->size < (u32)sizeof(s) ) \
        return MX_INVALID_INFO_SIZE; 

#define CONVERT_ID(id,ptr,idtype,cs,fn) \
    _MXASSERTE ( ID_TYPE(id) == idtype ); \
    if ( (u32)ID_TYPE(id) != (u32)idtype ) \
        return MX_INVALID_IDTYPE; \
    cs * ptr = static_cast<cs*>(fn ( GET_ID(id) ));  \
    _MXASSERT_VALID ( ptr ); \
    if ( ptr == NULL ) \
        return MX_INVALID_ID;


#define CONVERT_CHARACTER_ID(id,ptr) \
    CONVERT_ID(id,ptr,IDT_CHARACTER,mxcharacter,mx->CharacterById)
#define CONVERT_STRONGHOLD_ID(id,ptr) \
    CONVERT_ID(id,ptr,IDT_STRONGHOLD,mxstronghold,mx->StrongholdById)
#define CONVERT_ROUTENODE_ID(id,ptr) \
    CONVERT_ID(id,ptr,IDT_ROUTENODE,mxroutenode,mx->RouteNodeById)
#define CONVERT_REGIMENT_ID(id,ptr) \
    CONVERT_ID(id,ptr,IDT_REGIMENT,mxregiment,mx->RegimentById)
#define CONVERT_OBJECT_ID(id,ptr) \
    CONVERT_ID(id,ptr,IDT_OBJECT,mxobject,mx->ObjectById)

#define CONVERT_ENTITY_ID(id,ptr) \
    tme::mxentity* ptr = mx->EntityByIdt ( id );  \
    _MXASSERT_VALID ( ptr ); \
    if ( ptr == NULL ) \
        return MX_INVALID_ID;
    
#define CONVERT_DDR_CHARACTER_ID(id,ptr) \
CONVERT_ID(id,ptr,IDT_CHARACTER,ddr_character,mx->CharacterById)

#endif //_MIDNIGHT_H_INCLUDED_
