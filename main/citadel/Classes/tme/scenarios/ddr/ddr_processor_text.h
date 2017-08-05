#ifndef _CDDR_TEXTPROCESSOR_H_INCLUDED_
#define _CDDR_TEXTPROCESSOR_H_INCLUDED_

#include <string.h>

#include "../../baseinc/lomxtypes.h"
#include "../../baseinc/info.h"

using namespace tme ;
using namespace chilli;
using namespace chilli::collections;




namespace tme {

	class CStrBuf;
	
	//namespace processors {

    class ddr_text : public mxtext
		{

		public:
			ddr_text();
			virtual ~ddr_text();

            virtual void Serialize ( archive& ar );

            
            virtual LPSTR HowMuchOfText( u32 number, LPSTR text1, LPSTR text2 );
            virtual LPSTR DescribeNumberPart ( int number, ZERO_MODE zeromode=ZERO_NO )  ;
            virtual LPSTR DescribeCharacterTime( const mxcharacter* character );
            virtual LPSTR DescribeTime ( u32 time );
            virtual LPSTR DescribeCharacterDeath2 ( const mxcharacter* character );
            //virtual LPSTR DescribeEnergy ( u32 energy );
            
            // battles
            virtual LPSTR DescribeCharacterBattle ( const mxcharacter* character );

            virtual LPSTR DescribeVictory ( u32 victory, s32 value );
            LPSTR DescribeCharacterLocation( const mxcharacter* character );

            
			// strongholds

			virtual LPSTR DescribeStronghold (const mxstronghold* stronghold) ;

        public:
            LPSTR			victory_token[4];

            
            
		};
	//}
}



#endif //_CDDR_TEXTPROCESSOR_H_INCLUDED_
