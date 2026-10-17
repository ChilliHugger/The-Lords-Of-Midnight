#ifndef _COBJECTPROCESSOR_H_INCLUDED_
#define _COBJECTPROCESSOR_H_INCLUDED_

namespace tme {

    class mxobjectprocessor
    {
    public:
        mxobjectprocessor();
        virtual ~mxobjectprocessor();

        mxcharacter* WhoHasObject ( const mxobject* object ) const;
        mxobject* FindObjectAtLocation ( mxgridref loc );

        mxobject* Pickup ( mxcharacter* character );
        void Drop ( mxcharacter* character );

        void PlaceObject ( mxobject* object, mxgridref loc );
        void PlaceObjectsOnMap ( const mxcharacter* character );

        // false, and nothing changes, if there is no one to give to or their hands are full
        bool Give ( mxcharacter* from, mxcharacter* to );
        void Exchange ( mxcharacter* first, mxcharacter* second );
        void Destroy ( mxobject* object );

    private:
        void RemoveFromMap ( mxobject* object, mxgridref loc );
        bool AnotherLiesAt ( mxgridref loc ) const;
    };

}

#endif // _COBJECTPROCESSOR_H_INCLUDED_
