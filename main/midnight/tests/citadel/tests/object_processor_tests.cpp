//
//  object_processor_tests.cpp
//  citadel
//
//  Artefacts keep their own location, and a square is only flagged as having one.
//

#include "../../steps/tme_steps.h"
#include "../../steps/map_steps.h"

#include <filesystem>


namespace {

    const mxgridref k_here = mxgridref(10, 10);

    constexpr LPCSTR ob_stormblade = "OB_STORMBLADE";
    constexpr LPCSTR ob_bloodbringer = "OB_BLOODBRINGER";

    mxcharacter* LordWithEmptyHands(mxgridref loc)
    {
        MapStep::ResetLocation(loc);
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, loc, DR_NORTH);
        lord->CarryObject(nullptr);
        return lord;
    }

    void Lay(LPCSTR name, mxgridref loc)
    {
        auto object = GetObject(name);
        object->CarriedBy(nullptr);
        tme::mx->objectprocessor->PlaceObject(object, loc);
    }

}


SCENARIO("A lord can take an artefact lying where they stand, and drop it to be taken again")
{
    TMEStep::NewStory();

    GIVEN("an artefact lying where a lord stands")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto blade = GetObject(ob_stormblade);
        REQUIRE( blade->IsArtefact() );
        Lay(ob_stormblade, k_here);

        WHEN("the lord takes it")
        {
            REQUIRE( lord->Cmd_PickupObject() == blade );

            THEN("they carry it and it is no longer on the map")
            {
                REQUIRE( lord->Carrying() == blade );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(blade) == lord );
                REQUIRE_FALSE( tme::mx->gamemap->HasObject(k_here) );
            }

            AND_WHEN("they drop it")
            {
                lord->Cmd_DropObject();

                THEN("it lies there, and can be taken again")
                {
                    REQUIRE_FALSE( blade->IsCarried() );
                    REQUIRE( lord->Cmd_PickupObject() == blade );
                }
            }
        }
    }
}

SCENARIO("Lords swap artefacts")
{
    TMEStep::NewStory();

    GIVEN("two lords, each carrying an artefact")
    {
        auto first = LordWithEmptyHands(k_here);
        auto second = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);
        auto blade = GetObject(ob_stormblade);
        auto bringer = GetObject(ob_bloodbringer);
        first->CarryObject(blade);
        second->CarryObject(bringer);

        WHEN("they swap")
        {
            tme::mx->objectprocessor->Exchange(first, second);

            THEN("each carries what the other had")
            {
                REQUIRE( first->Carrying() == bringer );
                REQUIRE( second->Carrying() == blade );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(blade) == second );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(bringer) == first );
            }
        }
    }
}

SCENARIO("A destroyed artefact is no longer carried or on the map")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an artefact")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto blade = GetObject(ob_stormblade);
        lord->CarryObject(blade);

        WHEN("it is destroyed")
        {
            tme::mx->objectprocessor->Destroy(blade);

            THEN("they carry nothing, and it is nowhere")
            {
                REQUIRE( lord->Carrying() == nullptr );
                REQUIRE( blade->IsDestroyed() );
                REQUIRE_FALSE( static_cast<citadel_object*>(blade)->IsOnMap() );
            }
        }
    }
}

SCENARIO("What makes an object an artefact survives a saved game")
{
    TMEStep::NewStory();

    GIVEN("a saved game")
    {
        auto path = (std::filesystem::temp_directory_path() / "artefact_tests.sav").string();
        REQUIRE( tme::mx->SaveGame(path, nullptr) == MX_OK );

        u32 artefacts = 0;
        for ( auto object : tme::mx->objObjects )
            artefacts += object->IsArtefact() ? 1 : 0;
        REQUIRE( artefacts > 0 );

        WHEN("it is loaded after the objects have forgotten what they are")
        {
            for ( auto object : tme::mx->objObjects ) {
                object->type = OT_NONE;
                object->power = OP_NONE;
            }
            auto result = tme::mx->LoadGame(path, nullptr);
            std::filesystem::remove(path);

            THEN("the same objects are artefacts again")
            {
                u32 restored = 0;
                for ( auto object : tme::mx->objObjects )
                    restored += object->IsArtefact() ? 1 : 0;

                REQUIRE( result == MX_OK );
                REQUIRE( restored == artefacts );
            }
        }
    }
}

SCENARIO("An artefact is exported with its type and power")
{
    TMEStep::NewStory();

    GIVEN("an artefact")
    {
        auto artefact = GetObject(ob_stormblade);

        WHEN("it is asked for through the interface")
        {
            tme::object exported;
            REQUIRE( TME_GetObject(exported, MAKE_ID(IDT_OBJECT, artefact->Id())) );

            THEN("the type and power are the object's own")
            {
                REQUIRE( exported.type == artefact->type );
                REQUIRE( exported.power == artefact->power );
                REQUIRE( exported.type != OT_NONE );
                REQUIRE( exported.power != OP_NONE );
            }
        }
    }
}
