#ifndef _CDDR_TEXTPROCESSOR_H_INCLUDED_
#define _CDDR_TEXTPROCESSOR_H_INCLUDED_

#include <string.h>

#include "../../baseinc/lomxtypes.h"
#include "../../baseinc/info.h"

namespace tme {

    using namespace chilli;
    //using namespace chilli::collections;
    
    class CStrBuf;
    
    //namespace processors {

    class ddr_text : public mxtext
        {

        public:
            ddr_text();
            virtual ~ddr_text();

            virtual void Serialize ( archive& ar );

            
            virtual std::string HowMuchOfText( u32 number, LPSTR text1, LPSTR text2 );
            virtual std::string DescribeNumberPart ( int number, ZERO_MODE zeromode=ZERO_NO )  ;
            virtual std::string DescribeCharacterTime( const mxcharacter* character );
            virtual std::string DescribeTime ( u32 time );
            virtual std::string DescribeCharacterDeath2 ( const mxcharacter* character );
            //virtual std::string DescribeEnergy ( u32 energy );
            
            // battles
            virtual std::string DescribeCharacterBattle ( const mxcharacter* character );

            virtual std::string DescribeVictory ( u32 victory, s32 value );
            std::string DescribeCharacterLocation( const mxcharacter* character );

            
            // strongholds

            virtual std::string DescribeStronghold (const mxstronghold* stronghold) ;

        public:
            LPSTR            victory_token[4];

            
            
        };
    //}
}



#endif //_CDDR_TEXTPROCESSOR_H_INCLUDED_
