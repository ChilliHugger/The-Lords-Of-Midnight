#include "../../libinc/library.h"
#include "../../libinc/mxtypes.h"

namespace tme {

	namespace collections {
/*
		Idts::Idts(void)
		{
			m_count=0;
			m_idtElements=NULL;
		}

		Idts::~Idts(void)
		{
			Clear();
		}

		BOOL Idts::Create( u32 count )
		{
			Clear();
			m_count = count;
			if ( m_count )
				m_idtElements = new idt [ m_count ];
			return TRUE ;
		}

		u32 Idts::Count() const
		{ 
			return m_count;
		}

		idt& Idts::operator[]( u32 nSubscript )
		{
		static idt iErr = 0;

			if ( nSubscript>=0 && nSubscript<Count() )
				return m_idtElements[ nSubscript ];
			return iErr;
		}

		void Idts::Clear( void )
		{
			m_count=0;
			SAFEDELETEARRAY(m_idtElements);
		}
*/
	}
	// namespace collection
}
// namespace tme
