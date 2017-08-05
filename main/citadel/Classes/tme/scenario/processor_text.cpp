/*
 * FILE:    processor_text.cpp
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

#include "../baseinc/tme_internal.h"

#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

namespace tme {

CStrBuf::CStrBuf()
{
	cBytes=0;
	buffer=NULL;
	Resize(MAX_STRING_SIZE);
	mx->text->RememberStringBuffer( this );
	buffer[0]='\0';
}

CStrBuf::CStrBuf( u32 size )
{
	cBytes=0;
	buffer=NULL;
	Resize(size);
	mx->text->RememberStringBuffer( this );
	buffer[0]='\0';
}


CStrBuf::~CStrBuf()
{
	mx->text->ForgetStringBuffer( this );
	SAFEFREE(buffer);
	cBytes=0;
}

void CStrBuf::sprintf ( LPSTR format, ... )
{
va_list arglist ;
  	va_start( arglist, format ) ;
  	vsprintf( buffer, format, arglist );
  	va_end( arglist ) ;
}

void CStrBuf::sprintf ( CStrBuf* format, ... )
{
va_list arglist ;
  	va_start( arglist, format ) ;
  	vsprintf( buffer, format->GetAt(), arglist );
  	va_end( arglist ) ;
}

void CStrBuf::CheckSize ( size_t size )
{
	size_t newsize = Length() + size ;
	if ( newsize > cBytes ) {
		Resize(newsize+1);
	}
}

void CStrBuf::strcat ( LPCSTR input )		
{ 
	CheckSize(Length() + chilli::lib::strlen(input));
	chilli::lib::strcat( buffer, input ); 
}

void CStrBuf::strcpy ( LPCSTR input )		
{ 
	CheckSize(chilli::lib::strlen(input));
	chilli::lib::strcpy( buffer, input ); 
}
void CStrBuf::Append ( CStrBuf* input )	
{ 
	CheckSize(Length() + input->Length());
	chilli::lib::strcat( buffer, input->GetAt() ); 
}
void CStrBuf::Copy ( CStrBuf* input )
{ 
	CheckSize(input->Length());
	chilli::lib::strcpy( buffer, input->GetAt() ); 
}

void CStrBuf::Resize ( size_t size )
{
	buffer = (LPSTR)realloc(buffer, size ); 
	cBytes = size ;
}


//namespace processors {

/*
 * Function name	: text::text
 * 
 * Return type		: 
 * 
 * Arguments		: 
 * 
 * Description	    : 
 * 
 */

mxtext::mxtext()
{
	CLEARARRAY( stringbuffers );
	currentbuffer=0;
	systemstrings=NULL;
	systemcodes=NULL;
    m_case=CASE_NONE;
    last_number=0;

}


/*
 * Function name	: text::~text
 * 
 * Return type		: 
 * 
 * Arguments		: 
 * 
 * Description	    : 
 * 
 */

mxtext::~mxtext()
{
	PurgeCache(NULL);
	delete [] systemstrings ;
	delete [] systemcodes ;
}



/*
 * Function name	: text::PurgeCache
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: CStrBuf* string
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::PurgeCache( CStrBuf* string )
{
	if ( string==NULL ) {
		SAFEDELETE ( stringbuffers[0] );
		return NULL ;
	}

	SAFEDELETE ( stringbuffers[string->Id()+1] );
	return string->GetAt();
}


/*
 * Function name	: text::RememberStringBuffer
 * 
 * Return type		: void
 * 
 * Arguments		: CStrBuf* string
 * 
 * Description	    : 
 * 
 */

void mxtext::RememberStringBuffer ( CStrBuf* string )
{
	_ASSERTE ( currentbuffer<NUMELE(stringbuffers) );
	stringbuffers[currentbuffer]=string;
	string->Id(currentbuffer);
	currentbuffer++;
}


/*
 * Function name	: text::ForgetStringBuffer
 * 
 * Return type		: void
 * 
 * Arguments		: const CStrBuf* string
 * 
 * Description	    : 
 * 
 */

void mxtext::ForgetStringBuffer ( const CStrBuf* string )
{
//int id = string->Id();
int ii;
	for ( ii=currentbuffer-1;ii>string->Id(); ii-- ) {
		CStrBuf* temp = stringbuffers[ii] ;
		if ( temp ) {
			stringbuffers[ii] = NULL ;
			SAFEDELETE ( temp );
		}
	}
	currentbuffer=string->Id();
	stringbuffers[string->Id()]=NULL;
}


/*
 * Function name	: text::FillArrayFromSystemString
 * 
 * Return type		: void
 * 
 * Arguments		: LPSTR array[]
 *                  : int id
 * 
 * Description	    : 
 * 
 */

void mxtext::FillArrayFromSystemString( LPSTR array[], u32 id )
{
LPCSTR	token;
int		count;
	//set up arrays
	count=0;
	token = (LPCSTR)strtok( (LPSTR)SystemString(id), "|" );
	while ( token!=NULL ) {
		if ( chilli::lib::strlen(token)==1 && token[0]=='-' )
			array[count++] = NULL;
		else
			array[count++] = (LPSTR)token;
		token=strtok(NULL,"|");
	}
}

void mxtext::FillArrayFromSystemString( c_ptr& array, u32 id )
{
LPCSTR	token;
int		count;
	//set up arrays
	count=0;
	token = strtok( (LPSTR)SystemString(id), "|" );
	while ( token!=NULL ) {
		if ( chilli::lib::strlen(token)==1 && token[0]=='-' )
			array.Add(NULL);
		else
			array.Add((LPSTR)token);
		token=strtok(NULL,"|");
	}
}


/*
 * Function name	: text::Serialize
 * 
 * Return type		: void
 * 
 * Arguments		: archive& ar
 * 
 * Description	    : 
 * 
 */

void mxtext::Serialize ( archive& ar )
{
u32 ii;
int id;

	if ( ar.IsStoring() ) {
		ar << m_cSystemStrings;
		for ( ii=0; ii<m_cSystemStrings; ii++ ) {
			ar << ii;
			ar << systemcodes[ii];
			ar << systemstrings[ii];
		}
	}else{
		
		if ( systemstrings )
			delete [] systemstrings ;

		ar >> m_cSystemStrings;
		systemstrings = new string[m_cSystemStrings];
		systemcodes = new string[m_cSystemStrings];
		for ( ii=0; ii<m_cSystemStrings; ii++ ) {
			ar >> id;
			ar >> systemcodes[id] ;
			ar >> systemstrings[id];
		}

		FillArrayFromSystemString( adverb_token, SS_ADVERBS );
		FillArrayFromSystemString( number_token, SS_NUMBERS );
		FillArrayFromSystemString( fear_token, SS_FEARTOKENS );
		FillArrayFromSystemString( courage_token, SS_COURAGETOKENS );
		FillArrayFromSystemString( energy_token, SS_ENERGYTOKENS );
		FillArrayFromSystemString( zero_token, SS_ZEROTOKENS );

		FillArrayFromSystemString( despondent_token, SS_DESPONDENTTOKENS );
		FillArrayFromSystemString( reckless_token, SS_RECKLESSTOKENS );

		FillArrayFromSystemString( traits_token, SS_TRAITS );

		FillArrayFromSystemString( plural_tokens, SS_PLURALTOKENS );

	}
}


mxid mxtext::StringByName ( LPCSTR name ) const
{
	for ( u32 ii=0; ii<m_cSystemStrings; ii++ ) {
		if ( chilli::lib::stricmp(systemcodes[ii],name) == 0 ) {
			return MAKE_ID(IDT_STRING,(ii+1));
		}
	}

	return IDT_NONE;
}

LPCSTR mxtext::SystemStringById ( mxid id )
{
	if ( ID_TYPE(id) != IDT_STRING )
		return "" ;
	return SystemString(GET_ID(id)-1);
}


/*
 * Function name	: text::SystemString
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int id
 * 
 * Description	    : 
 * 
 */

LPCSTR mxtext::SystemString ( u32 id )
{
	//_ASSERTE ( id>=0 && id<m_cSystemStrings );
	if ( /*id<0 ||*/ id>=m_cSystemStrings ) return "" ;
	return systemstrings[id];
}


/*
 * Function name	: text::CookedSystemString
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int id
 *                  : const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::CookedSystemString ( u32 id, const mxcharacter* character )
{
	return CookText((LPSTR)SystemString(id),character) ;
}



/*
 * Function name	: text::SpecialStrings
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int id
 *                  : const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::SpecialStrings ( LPCSTR token, const mxcharacter* character )
{
	if ( chilli::lib::stricmp(token,"days") == 0 ) {
        
        CStrBuf*	buffer = new CStrBuf ;
        
        if ( sv_days > 999 ) {
            buffer->strcpy( "countless");
            last_number=sv_days;
        }else{
            buffer->sprintf ((LPSTR)"%s", DescribeNumber(sv_days));
        }
        return PurgeCache(buffer);
    }
	return (LPSTR)"{NONE}" ;
}

/*
 * Function name	: text::HowMuchOfText
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int number
 *                  : LPSTR text1
 *                  : LPSTR text2
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::HowMuchOfText( u32 number, LPSTR text1, LPSTR text2 )
{
CStrBuf*	buffer = new CStrBuf ;
    if ( number> NUMELE(adverb_token) )
        number=0;
    
	if (number < 5 ) text1 = text2;
	if ( adverb_token[number] ) 
		buffer->strcpy ( adverb_token[ number ] );
	buffer->strcat ( text1 );
	return buffer->GetAt();
}


/*
 * Function name	: text::DescribeNumber
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int number
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeNumber ( int number, ZERO_MODE zerotype )
{
CStrBuf*	buffer = new CStrBuf ;
    last_number=number;
	buffer->strcpy ( DescribeNumberPart(number,zerotype) );
	return PurgeCache(buffer);
}


/*
 * Function name	: text::DescribeNumberPart
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int number
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeNumberPart ( int number, ZERO_MODE zeromode )
{
BOOL	useand;
CStrBuf*	buffer = new CStrBuf ;

	useand = FALSE ;
	int thousands = number / 1000 ;
	number %= 1000 ;
	int hundreds = number / 100 ;
	number %= 100 ;
	int tens = number / 10 ;
	number %= 10 ;

	if ( thousands ) {
		buffer->strcpy ( DescribeNumberPart ( thousands,zeromode ) );
		buffer->strcat ( (LPSTR)" " );
		buffer->strcat ( number_token[28] );
		useand = TRUE ;
	}

	if ( hundreds ) {
		if ( useand ) buffer->strcat ( (LPSTR)" " );
		buffer->strcat ( DescribeNumberPart ( hundreds,zeromode ) );
		buffer->strcat ( (LPSTR)" " );
		buffer->strcat ( number_token[27] );
		useand = TRUE ;
	}

	if ( tens>1 ) {
		if ( useand ) buffer->strcat ( (LPSTR)" and " );
		buffer->strcat ( number_token[19+tens-2] );
		useand = FALSE ;
		if ( number == 0 )
			return buffer->GetAt() ;
		buffer->strcat ( (LPSTR)" " );
	}else if ( tens == 1 ) 
		number+=10;

	if ( useand ) {
		if ( number == 0 )
			return buffer->GetAt();
		buffer->strcat ( (LPSTR)" and " );
	}
	if ( number == 0 )
		buffer->strcat ( zero_token[zeromode] );
		else
		buffer->strcat ( number_token[number-1] );
	return buffer->GetAt() ;
}

#if defined(_DDR_)
/*
 *
 *
 */

LPSTR mxtext::DescribeTime ( u32 time )
{
    return DescribeNumber((time+1)/2);
}
#endif
    
LPSTR mxtext::DescribeEnergy ( u32 energy )
{
    LPSTR txtEnergy=HowMuchOfText ( energy/sv_energy_scale, energy_token[0], energy_token[1] );
    if ( energy<=sv_energy_cannot_continue) {
        CStrBuf*	buffer = new CStrBuf ;
       buffer->sprintf ( (LPSTR)SystemString(SS_CHARACTER_CANNOT_CONTINUE)
                         , txtEnergy
                         );
        return PurgeCache(buffer);
    }
    
    return txtEnergy;
}

LPSTR mxtext::DescribeDespondent ( u32 despondent )
{
	return HowMuchOfText ( despondent/sv_despondent_scale, despondent_token[0], despondent_token[1] );
}

LPSTR mxtext::DescribeReckless ( u32 reckless )
{
	return HowMuchOfText ( reckless/sv_reckless_scale, reckless_token[0], reckless_token[1] );
}

LPSTR mxtext::DescribeFear ( u32 fear )
{
	return HowMuchOfText ( 7-(fear/sv_fear_scale), fear_token[0], fear_token[1] );
}

LPSTR mxtext::DescribeCourage ( u32 courage )
{
	return HowMuchOfText ( courage/sv_courage_scale, courage_token[0], courage_token[1] );
}
/*
 *
 *
 */


/*
 * Function name	: text::DescribePlural
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: LPSTR single
 *                  : LPSTR plural
 *                  : int number
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribePlural( u32 token, int number )
{

	if ( ABS(number) > 1 ) 
		token++;

	return plural_tokens[token];
}

/*
 * Function name	: text::DescribeCharacterRecruitMen
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 *                  : const mxstronghold* stronghold
 *                  : int qty
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterRecruitMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )
{
CStrBuf*	buffer = new CStrBuf ;

	mxunitinfo* uinfo = mx->UnitById(stronghold->Type());
	buffer->sprintf (
		(LPSTR)SystemString(SS_RECRUITMEN),
		qty,(LPCSTR)uinfo->Name(),
		stronghold->Total(), (LPCSTR)uinfo->Name()
		);

	buffer->strcpy ( CookText(buffer->GetAt(),character) );
	return PurgeCache(buffer);
}

#if defined(_DDR_)
LPSTR mxtext::DescribeObjectLocation(mxobject* object)
{
    CStrBuf*	buffer = new CStrBuf ;
    mxcharacter* c = mx->scenario->WhoHasObject(object) ;
    
    // object not carried
    if ( c == NULL ) {
        buffer->sprintf (
                         (LPSTR)SystemString(SS_SEEK_MSG2),
                         DescribeObjectWithPower(object),
                         DescribeLocationWithPrep(object->Location(),NULL)
                         );
    }else{
        buffer->sprintf (
                         (LPSTR)SystemString(SS_SEEK_MSG3),
                         DescribeObjectWithPower(object)
                         );
        buffer->strcpy ( CookText(buffer->GetAt(),c) );
    }

	return PurgeCache(buffer);
}
#endif
    
/*
 * Function name	: text::DescribeCharacterPostMen
 *
 * Return type		: LPSTR
 *
 * Arguments		: const mxcharacter* character
 *                  : const mxstronghold* stronghold
 *                  : int qty
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterPostMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )
{
CStrBuf*	buffer = new CStrBuf ;
	mxunitinfo* uinfo = mx->UnitById(stronghold->Type());
	buffer->sprintf (
		(LPSTR)SystemString(SS_POSTMEN),
		qty,(LPCSTR)uinfo->Name(),
		stronghold->Total(),(LPCSTR)uinfo->Name()
		);

	buffer->strcpy ( CookText(buffer->GetAt(),character) );
	return PurgeCache(buffer);
}


/*
 * Function name	: text::DescribeCharacterTime
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterTime( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf ;
    int time = character->Time();

	if ( time == sv_time_night )
		buffer->strcpy ( SystemString(SS_ITISNIGHT) );
	else if ( time == sv_time_dawn )
		buffer->strcpy ( SystemString(SS_ITISDAWN) );
	else {
		if ( time&1 )
			buffer->strcpy ( CookedSystemString(SS_LESSTHAN, character) );
		else
			buffer->strcpy ( CookedSystemString(SS_HOURSOFDAY,character) );
	}
	return PurgeCache(buffer);
}


/*
 * Function name	: text::DescribeCharacterEnergy
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterEnergy ( const mxcharacter* character )
{
	return CookedSystemString(SS_CHARACTER_ENERGY,character);
}

/*
 * Function name	: text::DescribeCharacterCourage
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterCourage ( const mxcharacter* character )
{
	return CookedSystemString(SS_CHARACTER_COURAGE,character);
}

/*
 * Function name	: text::DescribeCharacterFear
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterFear ( const mxcharacter* character )
{
	return CookedSystemString(SS_CHARACTER_FEAR,character);
}


/*
 * Function name	: text::DescribeCharacterObject
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterObject ( const mxcharacter* character )
{
	if ( character->carrying == NULL )
		return CookedSystemString(SS_CARRYNOTHING,character);
	return CookedSystemString(SS_CARRYING,character);
}

LPSTR mxtext::DescribeObject ( const mxobject* object )
{
    const mxobject* temp = oinfo;
	oinfo = object;
	LPSTR text= CookText(oinfo->description,NULL);
    oinfo = temp ;
    return text;
}
    
#if defined(_DDR_)
LPSTR mxtext::DescribeObjectWithPower ( const mxobject* object )
{
    const mxobject* temp = oinfo;
    oinfo = object;
	CStrBuf*	buffer = new CStrBuf ;
	buffer->sprintf ( CookText((LPSTR)"{obj:text}, whose power is in {case:lower}{obj:power}")  );
    oinfo = temp ;
	return PurgeCache(buffer);
}
#endif

    /*
 * Function name	: text::DescribeCharacterDeath
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */
/*		Wolves slew him.
 *		He was killed in battle.
 */

LPSTR mxtext::DescribeCharacterDeath ( const mxcharacter* character )
{
	if ( character->killedbyobject )
		return CookedSystemString(SS_KILLED_OBJECT,character);
	CStrBuf*	buffer = new CStrBuf ;
	buffer->sprintf ( CookedSystemString(SS_KILLED_BATTLE,character), DescribeCharacterBattle(character) );
	return PurgeCache(buffer);
}

#if defined(_DDR_)
LPSTR mxtext::DescribeCharacterDeath2 ( const mxcharacter* character )
{
    if ( character->killedbyobject )
        return CookedSystemString(SS_KILLED_OBJECT,character);
    return CookedSystemString(SS_KILLED_BY,character);
}
#endif


/*
 * Function name	: text::DescribeCharacterBattle
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */
/*		In the Battle of Ashimar, Shadows lost 110 warriors and 10 riders.
 *		Shadows alone slew 35 of the enemy. 
 *		His warriors slew 290.
 *		His riders slew 100.
 *		The battle continues.
 *		Victory went to the free!
 */
LPSTR mxtext::DescribeCharacterBattle ( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf ;

	int temp=0;
	temp += character->warriors.Lost() ? 1 : 0 ;
	temp += character->riders.Lost() ? 2 : 0 ;
	
	switch ( temp ) {
		case 0:
			buffer->strcpy ( CookedSystemString(SS_BATTLE_NONE,character) );
			break;
		case 1:
			sprintf ( buffer->End(), CookedSystemString(SS_BATTLE_WARRIORS,character), 
				DescribeNumber(character->warriors.Lost()) );
			break;
		case 2:
			sprintf ( buffer->End(), CookedSystemString(SS_BATTLE_RIDERS,character), 
				DescribeNumber(character->riders.Lost()) );
			break;
		case 3:
			sprintf ( buffer->End(), CookedSystemString(SS_BATTLE_WARRIORS_RIDERS,character),  
				DescribeNumber(character->warriors.Lost(),ZERO_NO),
				DescribeNumber(character->riders.Lost(),ZERO_NO) );
			break;
	}

	sprintf ( buffer->End(), 
		CookedSystemString(SS_BATTLE_CHARACTER_SLEW,character),
		DescribeNumber(character->battleslew,ZERO_NONE) );

	if ( character->warriors.Killed() )
		sprintf ( buffer->End(), 
			CookedSystemString(SS_BATTLE_WARRIORS_SLEW,character), 
			DescribeNumber(character->warriors.Killed(),ZERO_NONE) );

	if ( character->riders.Killed() )
		sprintf ( buffer->End(), 
			CookedSystemString(SS_BATTLE_RIDERS_SLEW,character), 		
			DescribeNumber(character->riders.Killed(),ZERO_NONE) );

	if ( !character->IsInBattle() ) {
		if ( character->HasWonBattle() )
			buffer->strcat ( CookedSystemString(SS_BATTLE_VICTORY_FREE,character) );
		else
			buffer->strcat ( CookedSystemString(SS_BATTLE_VICTORY_ENEMY,character) );
	} else
		buffer->strcat ( CookedSystemString(SS_BATTLE_VICTORY_NONE,character) );

	return PurgeCache(buffer);
}


/*
 * Function name	: text::DescribeCharacterArmy
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */
/*		Luxor the Moonprince commands no warriors and no riders.
 *		His warriors are quite tired. His riders are quite tired.
 */


LPSTR mxtext::DescribeCharacterArmy ( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf ;

	int temp=0;
	temp += character->warriors.Total() ? 1 : 0 ;
	temp += character->riders.Total() ? 2 : 0 ;


	switch ( temp ) {
		case 0:
			buffer->strcat ( CookedSystemString(SS_ARMY_NONE,character) );
			break;
		case 1:
			sprintf ( buffer->End(), 
				CookedSystemString(SS_ARMY_WARRIORS,character),
				DescribeNumber(character->warriors.Total()),
				DescribeEnergy( character->warriors.Energy() )
				);

			break;
		case 2:
			sprintf ( buffer->End(), 
				CookedSystemString(SS_ARMY_RIDERS,character),
				DescribeNumber(character->riders.Total()),
				DescribeEnergy ( character->riders.Energy() )
				);
			break;
		case 3:
			sprintf ( buffer->End(), 
				CookedSystemString(SS_ARMY_WARRIORS_RIDERS,character),
				DescribeNumber(character->warriors.Total()),
				DescribeNumber(character->riders.Total()),
				DescribeEnergy ( character->warriors.Energy() ),
				DescribeEnergy ( character->riders.Energy() )
				);
			break;
	}

	return PurgeCache(buffer);
}




/*
 * Function name	: text::DescribeCharacterTraits
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeCharacterTraits ( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf(1024) ;

	u32 f = character->traits ;
	BOOL first = TRUE;

	for ( int ii=0; ii<32; ii++ ) {
		int i = f&1;

		if ( traits_token[ii] ) {

			if ( i ) {
				if ( !first ) 
					buffer->strcat( ", " );

				buffer->strcat( (LPSTR)traits_token[ii] );
				first = FALSE ;
			}

		}

		f>>=1;

	}

	return PurgeCache(buffer);
}

LPSTR mxtext::DescribeCharacterFoe ( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf ;
	if ( character->foe )
			buffer->strcat ( CookedSystemString(SS_FOE,character) );
	return PurgeCache(buffer);
}

LPSTR mxtext::DescribeCharacterLiege ( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf ;
	if ( character->liege )
			buffer->strcat ( CookedSystemString(SS_LIEGE,character) );
	return PurgeCache(buffer);
}

#if defined(_DDR_)
    
LPSTR mxtext::DescribeCharacterInBattle ( const mxcharacter* character )
{
    CStrBuf*	buffer = new CStrBuf ;
    
    if ( character->IsPreparingForBattle() )
        buffer->strcat ( CookedSystemString(SS_BATTLE_PREPARES_BATTLE,character) );
    
    return PurgeCache(buffer);
}
    
LPSTR mxtext::DescribeCharacterLoyalty ( const mxcharacter* character )
{
    CStrBuf*	buffer = new CStrBuf ;
    
    if ( character->loyalty )
        buffer->strcat ( CookedSystemString(SS_LOYAL_TO,character) );
    
    return PurgeCache(buffer);
}
    
#endif
    
LPSTR mxtext::DescribeCharacterGroup ( const mxcharacter* character )
{
    CStrBuf*	buffer = new CStrBuf ;
    
    if ( character->following )
        buffer->strcat ( CookedSystemString(SS_GROUP_FOLLOWER,character) );
    else if ( character->followers )
        buffer->strcat ( CookedSystemString(SS_GROUP_LEADER,character) );
    
    return PurgeCache(buffer);
}

 
    
/*
 * Function name	: text::DescribeCharacterLocation
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxcharacter* character
 * 
 * Description	    : 
 * 
 */
    
LPSTR mxtext::DescribeCharacterLocation( const mxcharacter* character )
{
CStrBuf*	buffer = new CStrBuf ;

	mxloc& here = mx->gamemap->GetAt ( character->Location() );
	mxgridref loc = mx->scenario->FindLookingTowards(character->Location(),character->Looking());
	mxloc& there = mx->gamemap->GetAt ( loc );
	
    if ( (here.area!=there.area) ||
        (here.IsInDomain()!=there.IsInDomain()) ||
        (here.terrain!=there.terrain) )
        {
            buffer->strcpy ( CookedSystemString(SS_MESSAGE6,character) );
        }else{
            buffer->strcpy ( CookedSystemString(SS_MESSAGE5,character) );
        }
	
	return PurgeCache(buffer);
}

#if defined(_DDR_)
//
// Luxor sees [the object of type][ and ]an underground entrance.
//
// {char:name} sees
// and
// and underground entrance
// .
LPSTR mxtext::DescribeCharacterSees ( const mxcharacter* character )
{
    CStrBuf*	buffer = new CStrBuf ;

    mxobject* object = mx->scenario->FindObjectAtLocation(character->Location());
    BOOL entrance = mx->gamemap->HasTunnelEntrance(character->Location());

    if ( object== NULL && !entrance )
        return PurgeCache(buffer);
    
    buffer->strcat( character->Shortname() );
    buffer->strcat(" sees ");
    if ( object ) {
        buffer->strcat("the ");
        buffer->strcat( DescribeObjectWithPower(object) );
    }
    if ( entrance ) {
        if ( object )
            buffer->strcat("and ");
        buffer->strcat("an underground entrance");
    }
    buffer->strcat(". ");
    
	return PurgeCache(buffer);
}

LPSTR mxtext::DescribeLocationWithPrep ( mxgridref loc, const mxcharacter* character )
{
    CStrBuf*	buffer = new CStrBuf ;
    
    mxgridref oldLoc = this->loc;
    
    this->loc = loc ;
    
    if ( character && character->IsInTunnel() ) {
        buffer->strcat("in the tunnel");
        return PurgeCache(buffer);
    }
    
    buffer->strcat( CookText((LPSTR)"{loc:terrain:prep} "));
    buffer->strcat( DescribeLocation(loc));
    
    this->loc = oldLoc ;
    
    return PurgeCache(buffer);
}
#endif
    
    
    
/*
 * Function name	: text::DescribeLocation
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: gridref loc
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeLocation(mxgridref loc )
{
mxloc&	mapsqr = mx->gamemap->GetAt(loc);
	this->loc = loc ;
	if ( mapsqr.IsInDomain() )
		return CookedSystemString(SS_LOCATION_DOMAIN);
	return CookedSystemString(SS_LOCATION_NOTDOMAIN);
}



/*
 * Function name	: text::DescribeTerrainPlural
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int terrain
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeTerrainPlural(mxterrain_t terrain)
{
CStrBuf*	buffer = new CStrBuf ;
	tinfo = mx->TerrainById( terrain );
	if ( !tinfo->IsPlural() )
		buffer->strcpy ( tinfo->Name() );
	else
		buffer->strcpy ( CookedSystemString(SS_TERRAIN_PLURAL) );
	return PurgeCache(buffer);
}


/*
 * Function name	: text::DescribeTerrainSingularPlural
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int terrain
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeTerrainSingularPlural(mxterrain_t terrain)
{
CStrBuf*	buffer = new CStrBuf ;
	tinfo = mx->TerrainById( terrain );
	if ( !tinfo->IsPlural() )
		buffer->strcpy ( CookedSystemString(SS_TERRAIN_SINGLE) );
	else
		buffer->strcpy ( CookedSystemString(SS_TERRAIN_PLURAL) );
	return PurgeCache(buffer);
}


/*
 * Function name	: text::DescribeArea
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: int area
 * 
 * Description	    : 
 * 
 */

LPSTR mxtext::DescribeArea(u32 area)
{
CStrBuf*	buffer = new CStrBuf ;
mxarea*		ainfo;
	ainfo = mx->AreaById( area );
	if ( (LPSTR)ainfo->prefix )
		buffer->strcpy ( CookedSystemString(SS_AREA_PREFIXED) );
	else
		buffer->strcpy( ainfo->Name() );
	return PurgeCache(buffer);
}

/*
 * Function name	: text::DescribeStronghold
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: const mxstronghold* stronghold
 * 
 * Description	    : 
 * 
 */
/*
 *  LOM:
 *  The stronghold holds x warriors of the free.
 *	They are slightly tired.
 *	The strongholds is owned by x.
 *
 *  DDR:
 *  The fortress of Carorthand holds then hundred, two score and ten fey riders.
 *  They are slightly tired.
 *  The fortress is owned by x who is loyal to they fey.
 */

LPSTR mxtext::DescribeStronghold(const mxstronghold* stronghold)
{
CStrBuf*	buffer = new CStrBuf ;
mxterrain*	tinfo;

	mxloc& mapsqr = mx->gamemap->GetAt ( stronghold->Location() );

	buffer->sprintf ( 
		CookedSystemString(SS_STRONGHOLD3),
		DescribeLocation(stronghold->Location()),
		DescribeNumber( stronghold->Total() ),
		(LPCSTR)mx->UnitById(stronghold->Type())->Name(),
		(LPCSTR)mx->RaceById(stronghold->OccupyingRace())->SoldiersName() );

	if ( stronghold->Owner() ) {
		tinfo = mx->TerrainById( mapsqr.terrain );

		int message = SS_STRONGHOLD2 ;
		mxcharacter* character=stronghold->Owner() ;

		if ( stronghold->Occupier() && stronghold->Occupier() != stronghold->Owner() ) { 
			message = SS_STRONGHOLD1 ;
			character=stronghold->Occupier();
		}

		sprintf ( buffer->End(), CookedSystemString(message), (LPCSTR)tinfo->Name(), (LPCSTR)character->Longname() );
	}
		
	return PurgeCache(buffer);
}


LPSTR mxtext::DescribeArmyTotal(const mxarmytotal* army)
{
CStrBuf*	buffer = new CStrBuf ;

			sprintf ( buffer->End(), 
				CookedSystemString(SS_WARRIORS_RIDERS,army->character),
				DescribeNumber(army->warriors),
				DescribeNumber(army->riders)
				);


	return PurgeCache(buffer);

}



/*
 * Function name	: text::CookText
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: LPSTR input
 *                  : const mxcharacter* character
 * 
 * Description	    : 
 * 
 */

     
    LPSTR strcat_upper ( LPSTR dst, LPCSTR src )
    {
        if ( src == NULL ) return dst;
        LPSTR cp = dst;
        while( *cp ) cp++;
        while( (*cp++ = toupper((*src++)) ) ) ;
        return( dst );
        
    }
    LPSTR strcat_lower ( LPSTR dst, LPCSTR src )
    {
        if ( src == NULL ) return dst;
        LPSTR cp = dst;
        while( *cp ) cp++;
        while( (*cp++ = tolower((*src++))) ) ;
        return( dst );
        
    }
    
LPSTR mxtext::CookText ( LPSTR input, const mxcharacter* character )
{
CStrBuf*	final = new CStrBuf ;
CStrBuf*	buffer = new CStrBuf(1024) ;
char		token[64];
LPSTR		is;
LPSTR		os;
int			t;
char		l;

	t=0;
	is = input;
	os = buffer->GetAt();
	*os='\0';

    
    if ( input!= NULL ) {
	for(;;) {
		l = *is++;
		// start of special token
		if ( l == '{' ) {
			t=0;
			while(*is!='}') {token[t++]=*is++;};
			token[t]='\0';
			is++;
			*os='\0';
			
            // NOTE: not size safe!!!!
			LPSTR result = DecodeToken(token,character) ;
            
            if ( chilli::lib::strlen(result)!=0  ) {
            
                if ( m_case == CASE_NONE ) {
                    chilli::lib::strcat ( os, result  );
                } else if ( m_case == CASE_FIRST ) {
                    os[0] = toupper(result[0]);
                    os[1] = '\0';
                    strcat_lower ( os+1, result+1  );
                } else if ( m_case == CASE_UPPER ) {
                    strcat_upper ( os, result  );
                } else if ( m_case == CASE_LOWER ) {
                    strcat_lower ( os, result  );
                }
                
                os+=chilli::lib::strlen(os);
                m_case=CASE_NONE;
            //}else{
            //
            }
			continue;
		}	
		*os++ = l;
		// end of input
		if ( l == '\0' ) {
			break;
		}
	}
    }
	final->Copy ( buffer );
	SAFEDELETE(buffer);
	return PurgeCache(final);
}

/*
 * Function name	: text::DecodeToken
 * 
 * Return type		: LPSTR
 * 
 * Arguments		: LPSTR token
 *                  : const mxcharacter* character
 * 
 * Description	    : 
 * 
 */
//enum TOKENS {
//	TOKEN_NONE=0,
//	TOKEN_FORMAT=1,
//	TOKEN_AREA,
//	TOKEN_GENDER,
//	TOKEN_DIRECTION,
//	TOKEN_CHARACTER,
//	TOKEN_LOCATION,
//	TOKEN_BATTLE,
//	TOKEN_OBJECT,
//	TOKEN_RACE,
//	TOKEN_STRING,
//	TOKEN_TERRAIN,
//	TOKEN_STRONGHOLD,
//	TOKEN_UNIT,
//	TOKEN_SPECIAL,
//};
//
//token_t str_tokens_1[] = {
//	{ "format",		TOKEN_FORMAT },
//	{ "area",		TOKEN_AREA },
//	{ "gender",		TOKEN_GENDER },
//	{ "dir",		TOKEN_DIRECTION },
//	{ "char",		TOKEN_CHARACTER },
//	{ "loc",		TOKEN_LOCATION },
//	{ "battle",		TOKEN_BATTLE },
//	{ "obj",		TOKEN_OBJECT },
//	{ "race",		TOKEN_RACE },
//	{ "str",		TOKEN_STRING },
//	{ "terrain",	TOKEN_TERRAIN },
//	{ "stronghold",	TOKEN_STRONGHOLD },
//	{ "unit",		TOKEN_UNIT },
//	{ "special",	TOKEN_SPECIAL },
//};


LPSTR mxtext::DecodeToken ( LPSTR token, const mxcharacter* character )
{
int				is=0;
c_ptr			tokens;

	LPSTR t = strtok(token,"{:}");
	while ( t ) {
		tokens.Add(t);
		t = strtok(NULL,"{:}");
	}

	//if ( tokens.Count() == 0 )
	//	return NULL ;

	if ( character==NULL )
		character=mx->CurrentChar();

	if ( character ) {
		ginfo = mx->GenderById(character->gender);
		dinfo = mx->DirectionById(character->Looking());
		rinfo = mx->RaceById(character->Race());
	}

	ainfo = mx->AreaById( mx->gamemap->GetAt(loc).area );
	tinfo = mx->TerrainById( mx->gamemap->GetAt(loc).terrain );

	is=0;

#define GET_ARG	\
    (LPCSTR)tokens[is]
    
#define IS_ARG(x)	\
	 if ( chilli::lib::stricmp(x,GET_ARG) == 0 )

	while (TRUE) {
		if ( tokens[is] == NULL )
			return (LPSTR)"";

        
        
// case
            IS_ARG("case") {
                is++;
                IS_ARG("first")     { m_case=CASE_FIRST; return (LPSTR)""; }
                IS_ARG("upper")     { m_case=CASE_UPPER; return (LPSTR)""; }
                IS_ARG("lower")     { m_case=CASE_LOWER; return (LPSTR)""; }
                IS_ARG("none")      { m_case=CASE_NONE; return (LPSTR)""; }
            }else
            IS_ARG("plural") {
                is++;
                if ( ABS(last_number)==1 ) {
                    return (LPSTR)GET_ARG;
                }
                is++;
                return (LPSTR)GET_ARG;
            }else
                
			IS_ARG("number") {
                is++;
                return DescribeNumber(atoi(GET_ARG));
            }else
 // Character
			IS_ARG("char") {
__char:
				is++;
				IS_ARG("name")		return character->Shortname();
				IS_ARG("longname")	return character->Longname();
#if defined(_DDR_)
                IS_ARG("time")		return DescribeTime(character->Time());
#else
                IS_ARG("time")		return DescribeNumber((character->Time()+1)/2);
#endif
                IS_ARG("energy")	return DescribeEnergy(character->energy);
				IS_ARG("despondency")	return DescribeDespondent ( character->despondency );
				IS_ARG("reckless")		return DescribeReckless ( character->reckless );

				IS_ARG("fear")		{  
									character->ForcedVariableRefresh();
									return DescribeFear ( character->fear );
									}
				IS_ARG("courage")	{
									character->ForcedVariableRefresh();
									return DescribeCourage(character->courage);
									}

				IS_ARG("foe")		{
									character = character->foe;
									goto __char;
									}
				IS_ARG("liege")		{
									character = character->liege;
									goto __char;
									}
				IS_ARG("following")	{
                                    character = character->following;
                                    goto __char;
                                    }
#if defined(_DDR_)
				IS_ARG("loyalty")	{
									rinfo = mx->RaceById(character->Loyalty());
                                    goto __race;
                                    }
#endif
				IS_ARG("text")		{
					is++;
					IS_ARG("liege")		return DescribeCharacterLiege( character );
					IS_ARG("foe")		return DescribeCharacterFoe( character );
					IS_ARG("traits")	return DescribeCharacterTraits( character );
					IS_ARG("time")		return DescribeCharacterTime( character );
					IS_ARG("energy")	return DescribeCharacterEnergy ( character );
					IS_ARG("courage")	return DescribeCharacterCourage ( character );
					IS_ARG("fear")		return DescribeCharacterFear ( character );
					IS_ARG("obj")		return DescribeCharacterObject ( character );
					IS_ARG("death")		return DescribeCharacterDeath ( character );
					IS_ARG("battle")	return DescribeCharacterBattle ( character );
					IS_ARG("army")		return DescribeCharacterArmy ( character );
					IS_ARG("loc")		return DescribeCharacterLocation( character );
					IS_ARG("group")		return DescribeCharacterGroup( character );
#if defined(_DDR_)
                    IS_ARG("death2")	return DescribeCharacterDeath2 ( character );
					IS_ARG("loyalty")	return DescribeCharacterLoyalty( character );
					IS_ARG("sees")		return DescribeCharacterSees( character );
                    IS_ARG("inbattle")  return DescribeCharacterInBattle( character );
#endif
				}
			
				IS_ARG("lookat")	{
									loc = mx->scenario->FindLookingTowards(character->Location(),character->Looking());
									goto __loc;
									}
				IS_ARG("battle")	{
					is++;
					IS_ARG("loc")	{
									loc = character->battleloc;
									goto __loc;
									}
					IS_ARG("obj")	{
									oinfo = character->killedbyobject; 
									goto __obj;
									}
#if defined(_DDR_)
					IS_ARG("fighting")	{
                                    character = (mxcharacter*)((ddr_character*)character)->fighting_against;
                                    goto __char;
                                    }
#endif
                }
                
				IS_ARG("loc")		{
									loc = character->Location();
									goto __loc;
									}
				IS_ARG("gender")	{
									ginfo = mx->GenderById(character->gender);
									goto __gender;
									}
				IS_ARG("dir")		{
									dinfo = mx->DirectionById(character->Looking());
									goto __dir;
									}
				IS_ARG("obj")		{
									oinfo = character->carrying; //mx->ObjectById(character->object);
									goto __obj;
									}
				IS_ARG("race")		{
									rinfo = mx->RaceById(character->Race());
									goto __race;
									}
			}else
// RACE
			IS_ARG("race") {
__race:
				is++;
				IS_ARG("name")		return rinfo->Name();
				IS_ARG("soldiers")	return rinfo->SoldiersName();
			}else
// AREA
			IS_ARG("area") {
__area:
				is++;
				IS_ARG("name")		return ainfo->Name();
				IS_ARG("prefix")	return ainfo->prefix;
				IS_ARG("text")		return DescribeArea(ainfo->Id());
			}else
// GENDER
			IS_ARG("gender") {
__gender:
				is++;
				IS_ARG("name")		return ginfo->Name();
				IS_ARG("heshe")		return ginfo->pronoun1;
				IS_ARG("hisher")	return ginfo->pronoun2;
				IS_ARG("himher")	return ginfo->pronoun3;
			}else
// direction
			IS_ARG("dir") {
__dir:
				is++;
				IS_ARG("name")		return dinfo->Name();
			}else
//OBJECT
			IS_ARG("obj") {
__obj:
				is++;
				IS_ARG("name")		return oinfo->name;
						// if oinfo->description == NULL
						// SS_OBJECT_FULL_DESCRIPTION
				IS_ARG("text")		return CookText(oinfo->description,character);
#if defined(_DDR_)
                IS_ARG("fullname")  return DescribeObjectWithPower(oinfo);
				IS_ARG("type")		{
                                    otinfo = mx->ObjectTypeById( oinfo->type );
                                    return otinfo->Name();
                                    }
				IS_ARG("power")		{
                                    opinfo = mx->ObjectPowerById( oinfo->power );
                                    return opinfo->Name();
                                    }
#endif
			}else

// LOCATION
			IS_ARG("loc") {
__loc:
				is++;
				IS_ARG("name")		return DescribeLocation(loc);
#if defined(_DDR_)
				IS_ARG("text")		return DescribeLocationWithPrep(loc,character);
#endif
				IS_ARG("terrain")	{
									tinfo = mx->TerrainById( mx->gamemap->GetAt(loc).terrain );
									goto __terrain;
									}
				IS_ARG("obj")	{
									oinfo = mx->ObjectById( mx->gamemap->GetAt(loc).object );
									goto __obj;
									}
				IS_ARG("area")		{	
									ainfo = mx->AreaById( mx->gamemap->GetAt(loc).area );
									goto __area;
									}
			}else
// TERRAIN
			IS_ARG("terrain") {
__terrain:
				is++;
				IS_ARG("name")		return tinfo->Name();
				IS_ARG("plural")	return DescribeTerrainPlural((mxterrain_t)tinfo->Id());
				IS_ARG("prep")		return tinfo->preposition;
				IS_ARG("text")		return CookText(tinfo->description,character);
				IS_ARG("single")	return DescribeTerrainSingularPlural((mxterrain_t)tinfo->Id());
			}else
// SYSTEM STRING
			IS_ARG("str") {
				return CookedSystemString(chilli::lib::atol((LPCSTR)tokens[is+1]),character);
			}else
// SPECIAL
			IS_ARG("special") {
				return SpecialStrings((LPCSTR)tokens[is+1],character);
			}
		is++;
	}
}


//}
// namespace processors

}
// namespace tme

//abcd(e)fg(hijk)l(mn)o(pq)rstu(vwx)yz

/*
Text Formatting
{f1}	:	uppercase letter
{f2}	:	uppercase word

Area
{a1}	:	Name
{a2}	:	Prefix
{a3}	:	text

Gender
{g1}	:	Name
{g2}	:	Pronoun1 ( he/she )
{g3}	:	Pronoun2 ( his/her )
{g4}	:	Pronoun3 ( him/her )

Direction
{d1}	:	Name

Character
{c1}	:	name - Shortname
{c2}	:	Longname
{c3}	:	time
{c4}	:	energy
{c5}	:	fear
{c6}	:	courage
text
	{c7}	:	Describe Time
	{c8}	:	Describe Energy
	{c9}	:	Describe Courage
	{c10}	:	Describe Fear
	{c11}	:	Describe Object Carrying
	{c12}	:	Describe Death
	{c13}	:	Describe Battle
	{c14}	:	Describe Army
	{c15}	:	Describe Location/Looking


{cl*}	:	location
{cml*}	:	location looking at
{cbl*}	:	battle location
{cg*}	:	gender
{cd*}	:	direction
{co*}	:	object
{cr*}	:	race
	
Object
{o1}	:	name
{o2}	:	Found Description
{o3}	:	object type
{o4}	:	object power
//{o5}	:	full description

Race
{r1}	:	Name
{r2}	:	Soldiers Name

Strings
{zx}	:	System Strings. where x is a number

Location
{l1}	:	name
{lt*}	:	terrain
{lo*}	:	object
{la*}	:	area


Terrain
{t1}	:	Name
{t2}	:	Plural Safe Name
{t3}	:	Preposition
{t4}	:	description
{t5}	:	single 'a terrain'


Strongholds
{s1}	:	Name
{sc*}	:	owner
{sr*}	:	race
{sl*}	:	location

Units
{u1}	:	Name

{y}		: Special
*/
