#ifndef _CNIGHTPROCESSOR_H_INCLUDED_
#define _CNIGHTPROCESSOR_H_INCLUDED_

namespace tme {
	//namespace processors {

		class mxnight
		{
		public:
			mxnight();
			virtual ~mxnight();

			virtual void Process(void);
			virtual m_gameover_t CheckWinLoseConditions ( BOOL night );

            virtual void MoveMidwinter ( void );
            
#ifdef _TEST_WINLOSE_CONDITIONS_
            virtual void testWinLoseConditions ( void );
#endif
            
		private:
			void ResetLocationSpecial ( mxgridref loc );
            

		};
	//}
}

#endif //_CNIGHTPROCESSOR_H_INCLUDED_
