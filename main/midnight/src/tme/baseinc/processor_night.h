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

    private:
        void ResetLocationSpecial ( mxgridref loc );
    };

}

#endif //_CNIGHTPROCESSOR_H_INCLUDED_
