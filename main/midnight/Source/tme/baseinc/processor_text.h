#ifndef _CTEXTPROCESSOR_H_INCLUDED_
#define _CTEXTPROCESSOR_H_INCLUDED_

#include <string.h>

#include "lomxtypes.h"
#include "info.h"

#define    MAX_STRING_BUFFERS    64
#define MAX_STRING_SIZE        512


namespace tme {

    using namespace chilli;


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
            mxid StringByName ( const std::string& name ) const;
            void ModifySystemString( mxid id, std::string& value );

            std::string CookedSystemString ( u32 id, const mxcharacter* character=NULL );
            std::string CookText ( std::string& input, const mxcharacter* character=NULL );
            std::string DescribePlural( u32 single, int number );
            std::string DescribeNumber ( int number, ZERO_MODE zeromode=ZERO_NO );

            virtual void Serialize ( chilli::lib::archive& ar );

            virtual std::string DecodeToken ( LPSTR token, const mxcharacter* character=NULL );
            virtual std::string HowMuchOfText( u32 number, const c_string& tokens ) ;
            virtual std::string DescribeNumberPart ( int number, ZERO_MODE zeromode=ZERO_NO )  ;
            virtual std::string DescribeEnergy ( u32 energy );
            virtual std::string DescribeFear ( u32 fear );
            virtual std::string DescribeCourage ( u32 courage );
            virtual std::string DescribeDespondent ( u32 despondent );
            virtual std::string DescribeReckless ( u32 reckless );
            
            // character descriptions
            virtual std::string DescribeCharacterRecruitMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )  ;
            virtual std::string DescribeCharacterPostMen ( const mxcharacter* character, const mxstronghold* stronghold, u32 qty )  ;

            virtual std::string DescribeCharacterTime( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterEnergy ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterCourage ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterFear ( const mxcharacter* character ) ;

            virtual std::string DescribeCharacterObject ( const mxcharacter* character )  ;
            virtual std::string DescribeCharacterDeath ( const mxcharacter* character )  ;
            virtual std::string DescribeCharacterBattle ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterArmy ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterTraits ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterFoe ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterLiege ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterLocation( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterGroup ( const mxcharacter* character ) ;
            virtual std::string DescribeCharacterSees ( const mxcharacter* character );

            // strongholds

            virtual std::string DescribeStronghold (const mxstronghold* stronghold) ;

            // location
            virtual std::string DescribeLocation( mxgridref loc) ;
            virtual std::string DescribeLocationWithPrep ( mxgridref loc, const mxcharacter* character );
            virtual std::string DescribeArea(u32 area) ;
            virtual std::string DescribeTerrainPlural(mxterrain_t terrain);
            virtual std::string DescribeTerrainSingularPlural(mxterrain_t terrain);

            // objects
            virtual std::string DescribeObject ( const mxobject* object );

            // special strings
            virtual std::string SpecialStrings ( LPCSTR token, const mxcharacter* character );

        protected:
            c_string FillArrayFromSystemString(u32 id);

        public:
            mxgridref       loc;

            mxrace*         rinfo;
            mxgender*       ginfo;
            mxdirection*    dinfo;
            mxarea*         ainfo;
            mxterrain*      tinfo;
            const           mxobject*    oinfo;

            c_string        adverb_token;
            c_string        number_token;
            c_string        fear_token;
            c_string        courage_token;
            c_string        energy_token;
            c_string        reckless_token;
            c_string        despondent_token;
            c_string        zero_token;
            c_string        plural_tokens;
            c_string        traits_token;
            
            MODE_CASE       m_case;

            int             last_number;
        protected:
            u32             m_cSystemStrings;
            c_string        systemstrings;
            c_string        systemcodes;
            std::string     emptyString;
        };
    //}
}








#endif //_CTEXTPROCESSOR_H_INCLUDED_
