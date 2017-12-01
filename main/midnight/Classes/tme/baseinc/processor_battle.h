#ifndef _CBATTLEPROCESSOR_H_INCLUDED_
#define _CBATTLEPROCESSOR_H_INCLUDED_

#include "info.h"

#define MAX_FOE_ARMIES			128
#define	MAX_FRIEND_ARMIES		MAX_FOE_ARMIES


namespace tme {
	//namespace processors {

		class mxbattle  
		{
		public:
			mxbattle();
			virtual ~mxbattle();

			virtual void KickOffAtLocation ( mxgridref loc );
			
            virtual s32 BaseDoomdarkSuccess ( mxrace_t race, mxunit_t unit, const mxlocinfo& locinfo );
			virtual void ResetBattlesFought();

			enum INITIATIVE	{
				BI_CHARACTER	= 1,
				BI_FRIEND_1		= 2,
				BI_FOE_1		= 3,
				BI_FRIEND_2		= 4,
				BI_FOE_2		= 5,
			} INITIATIVE ;

		private:
			virtual void CalculateCharactersKillRates( void );
			virtual void MakeFriendOrFoeList ( const mxcharacter* character=NULL );
			virtual u32 Fight ( u32 attacks, u32 success, mxarmy* foes[], u32& nFoes );

			virtual void AllOutWar ( int pass );
			virtual void CheckVictors ( void );

		private:

			mxlocinfo*	info;
			u32			nFoes;
			u32			nFriends;
			mxarmy*     foes[MAX_FOE_ARMIES];
			mxarmy*     friends[MAX_FRIEND_ARMIES];
			u32			battlesfought;
			bitarray	battleareas;

		};

	//}
}

#endif //_CBATTLEPROCESSOR_H_INCLUDED_
