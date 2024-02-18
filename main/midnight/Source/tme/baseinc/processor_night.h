#ifndef _CNIGHTPROCESSOR_H_INCLUDED_
#define _CNIGHTPROCESSOR_H_INCLUDED_

namespace tme {

    class mxnight
    {
    public:
        mxnight();
        virtual ~mxnight();

        virtual void Process(void);

        virtual void MoveMidwinter ();

    protected:
        virtual void SetSpecialLocationsCharacter();
        virtual void SetSpecialLocationsStrongholds();
        virtual void ResetLocationSpecial ( mxgridref loc );
        virtual void ResetSpecialLocations();
    };

}

#endif //_CNIGHTPROCESSOR_H_INCLUDED_
