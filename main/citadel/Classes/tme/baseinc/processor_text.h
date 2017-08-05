#ifndef _CTEXTPROCESSOR_H_INCLUDED_
#define _CTEXTPROCESSOR_H_INCLUDED_

#include <string.h>

#include "lomxtypes.h"
#include "info.h"

#define	MAX_STRING_BUFFERS	64
#define MAX_STRING_SIZE		512

using namespace tme ;
using namespace chilli;
using namespace chilli::collections;




namespace tme {

	class CStrBuf;
	
	//namespace processors {

		class mxtext
		{
        public:
            enum MODE_CASE {
                CASE_NONE=0,
                CASE_FIRST=1,
                CASE_UPPER=2,
                CASE_LOWER=3
            };
            
            enum ZERO_MODE {
                ZERO_NO=0,
                ZERO_NONE=1,
                ZERO_ZERO=2,
            };
            
		public:
			mxtext();
			virtual ~mxtext();

			LPCSTR SystemString ( u32 id );
			LPCSTR SystemStringById ( mxid id );
			mxid StringByName ( LPCSTR name ) const;

			LPSTR CookedSystemString ( u32 id, const mxcharacter* character=NULL );
			LPSTR CookText ( LPSTR input, const mxcharacter* character=NULL );
			LPSTR DescribePlural( u32 single, int number );
			LPSTR DescribeNumber ( int number, ZERO_MODE zeromode=ZERO_NO );

			LPSTR PurgeCache( CStrBuf* string=NULL );
			void RememberStringBuffer ( CStrBuf* string );
			void ForgetStringBuffer ( const CStrBuf* string );
			virtual void Serialize ( chilli::lib::archive& ar );

			virtual LPSTR DecodeToken ( LPSTR token, const mxcharacter* character=NULL );
			virtual LPSTR HowMuchOfText( u32 number, LPSTR text1, LPSTR text2 ) ;
			virtual LPSTR DescribeNumberPart ( int number, ZERO_MODE zeromode=ZERO_NO )  ;
			virtual LPSTR DescribeEnergy ( u32 energy );
			virtual LPSTR DescribeFear ( u32 fear );
			virtual LPSTR DescribeCourage ( u32 courage );
			virtual LPSTR DescribeDespondent ( u32 despondent );
			virtual LPSTR DescribeReckless ( u32 reckless );
#if defined(_DDR_)
            virtual LPSTR DescribeTime ( u32 time );
#endif
            
			// character descriptions
			virtual LPSTR DescribeCharacterRecruitMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )  ;
			virtual LPSTR DescribeCharacterPostMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )  ;

			virtual LPSTR DescribeCharacterTime( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterEnergy ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterCourage ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterFear ( const mxcharacter* character ) ;

			virtual LPSTR DescribeCharacterObject ( const mxcharacter* character )  ;
			virtual LPSTR DescribeCharacterDeath ( const mxcharacter* character )  ;
#if defined(_DDR_)
			virtual LPSTR DescribeCharacterDeath2 ( const mxcharacter* character )  ;
#endif
			virtual LPSTR DescribeCharacterBattle ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterArmy ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterTraits ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterFoe ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterLiege ( const mxcharacter* character ) ;
			virtual LPSTR DescribeCharacterLocation( const mxcharacter* character ) ;
            virtual LPSTR DescribeCharacterGroup ( const mxcharacter* character ) ;
#if defined(_DDR_)
            virtual LPSTR DescribeCharacterLoyalty ( const mxcharacter* character ) ;
            virtual LPSTR DescribeCharacterSees ( const mxcharacter* character ) ;
            virtual LPSTR DescribeCharacterInBattle ( const mxcharacter* character );
#endif

			// strongholds

			virtual LPSTR DescribeStronghold (const mxstronghold* stronghold) ;

			// army totals
			virtual LPSTR DescribeArmyTotal(const mxarmytotal* army) ;

			// location
			virtual LPSTR DescribeLocation( mxgridref loc) ;
#if defined(_DDR_)
            virtual LPSTR DescribeLocationWithPrep ( mxgridref loc, const mxcharacter* character );
#endif
            virtual LPSTR DescribeArea(u32 area) ;
			virtual LPSTR DescribeTerrainPlural(mxterrain_t terrain);
			virtual LPSTR DescribeTerrainSingularPlural(mxterrain_t terrain);

			// objects
			virtual LPSTR DescribeObject ( const mxobject* object );
#if defined(_DDR_)
            virtual LPSTR DescribeObjectLocation( mxobject* object);
            virtual LPSTR DescribeObjectWithPower ( const mxobject* object );
#endif


			// special strings
			virtual LPSTR SpecialStrings ( LPCSTR token, const mxcharacter* character );

		protected:
			void FillArrayFromSystemString( LPSTR array[], u32 id );
			void FillArrayFromSystemString( c_ptr& array, u32 id );

		public:
			CStrBuf*		stringbuffers[MAX_STRING_BUFFERS];
			int				currentbuffer;
			mxgridref		loc;

			mxrace*			rinfo;
			mxgender*		ginfo;
			mxdirection*	dinfo;
			mxarea*			ainfo;
			mxterrain*		tinfo;
#if defined(_DDR_)
            mxobjectpower*  opinfo;
            mxobjecttype*   otinfo;
#endif
			const           mxobject*	oinfo;

#if defined(_DDR_)
			LPSTR			adverb_token[8];
#else
			LPSTR			adverb_token[9];
#endif
			LPSTR			number_token[30];
			LPSTR			fear_token[2];
			LPSTR			courage_token[2];
			LPSTR			energy_token[2];
			LPSTR			reckless_token[2];
			LPSTR			despondent_token[2];
#if defined(_DDR_)
			LPSTR			zero_token[3];
#endif
#if defined(_LOM_)
            LPSTR			zero_token[10];
#endif
            LPSTR			plural_tokens[4];

			c_ptr			traits_token;
            
			//u32				m_zerotype;
            
            MODE_CASE       m_case;

            
            int             last_number;
		protected:
			u32				m_cSystemStrings;
			string*			systemstrings;
			string*			systemcodes;
		};
	//}
}








#endif //_CTEXTPROCESSOR_H_INCLUDED_
