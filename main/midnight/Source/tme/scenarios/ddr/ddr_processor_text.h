#ifndef _CDDR_TEXTPROCESSOR_H_INCLUDED_
#define _CDDR_TEXTPROCESSOR_H_INCLUDED_

#include <string.h>

#include "../../baseinc/lomxtypes.h"
#include "../../baseinc/info.h"

namespace tme {

#if defined(_DDR_)
    using namespace chilli;
    class CStrBuf;
    
    class ddr_text : public mxtext
        {

        public:
            ddr_text();
            virtual ~ddr_text();

            virtual void Serialize ( archive& ar );

            
            virtual std::string HowMuchOfText( u32 number, const c_string& tokens );
            virtual std::string DescribeNumberPart ( int number, ZERO_MODE zeromode=ZERO_NO )  ;
            virtual std::string DescribeTime ( u32 time );

            virtual std::string DescribeCharacterTime( const mxcharacter* character );
            virtual std::string DescribeCharacterDeath ( const mxcharacter* character );
            virtual std::string DescribeCharacterDeath2 ( const mxcharacter* character );
            virtual std::string DescribeCharacterInBattle ( const mxcharacter* character );
            virtual std::string DescribeCharacterLoyalty ( const mxcharacter* character );
            virtual std::string DescribeCharacterLocation( const mxcharacter* character );
            virtual std::string DescribeCharacterSees ( const mxcharacter* character );
            
            virtual std::string DescribeLocationWithPrep ( mxgridref loc, const mxcharacter* character );
            
            virtual std::string DescribeObjectLocation( mxobject* object);
            virtual std::string DescribeObjectWithPower ( const mxobject* object );

            
            // battles
            virtual std::string DescribeCharacterBattle ( const mxcharacter* character );

            virtual std::string DescribeVictory ( u32 victory, s32 value );

            
            // strongholds
            virtual std::string DescribeStronghold (const mxstronghold* stronghold) ;

        public:
            c_string        victory_token;
            mxobjectpower*  opinfo;
            mxobjecttype*   otinfo;
        };
#endif
}

#endif //_CDDR_TEXTPROCESSOR_H_INCLUDED_
