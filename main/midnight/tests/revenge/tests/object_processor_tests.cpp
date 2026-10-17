//
//  object_processor_tests.cpp
//  revenge
//
//  Artefacts keep their own location, and a square is only flagged as having one.
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"

#include <filesystem>
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(10, 10);
    const mxgridref k_there = mxgridref(10, 11);

    constexpr LPCSTR ob_crown = "OB_CROWN_VARENAND";
    constexpr LPCSTR ob_other_crown = "OB_CROWN_CARUDRIUM";

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


SCENARIO("A lord can take an artefact lying where they stand")
{
    TMEStep::NewStory();

    GIVEN("an artefact lying where a lord stands")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto crown = GetObject(ob_crown);
        REQUIRE( crown->IsArtefact() );
        Lay(ob_crown, k_here);

        WHEN("the lord takes it")
        {
            auto taken = lord->Cmd_PickupObject();

            THEN("they carry it, it knows who has it, and the square has nothing left")
            {
                REQUIRE( taken == crown );
                REQUIRE( lord->Carrying() == crown );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(crown) == lord );
                REQUIRE_FALSE( tme::mx->gamemap->HasObject(k_here) );
            }
        }
    }
}

SCENARIO("An artefact that has been dropped can be taken again")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an artefact")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto crown = GetObject(ob_crown);
        lord->CarryObject(crown);

        WHEN("they drop it and take it back")
        {
            lord->Cmd_DropObject();

            THEN("it lies there, uncarried")
            {
                REQUIRE( crown->Location() == k_here );
                REQUIRE_FALSE( crown->IsCarried() );
                REQUIRE( tme::mx->gamemap->HasObject(k_here) );
            }

            AND_THEN("it can be taken again")
            {
                REQUIRE( lord->Cmd_PickupObject() == crown );
                REQUIRE( lord->Carrying() == crown );
            }
        }
    }
}

SCENARIO("Taking one of two artefacts leaves the other to be found")
{
    TMEStep::NewStory();

    GIVEN("two artefacts lying in the same place")
    {
        auto lord = LordWithEmptyHands(k_here);
        Lay(ob_crown, k_here);
        Lay(ob_other_crown, k_here);

        WHEN("the lord takes one")
        {
            auto taken = lord->Cmd_PickupObject();

            THEN("the other is still there, and the square is still flagged")
            {
                REQUIRE( taken != nullptr );
                REQUIRE( tme::mx->gamemap->HasObject(k_here) );
                auto left = tme::mx->objectprocessor->FindObjectAtLocation(k_here);
                REQUIRE( left != nullptr );
                REQUIRE( left != taken );
            }
        }
    }
}

SCENARIO("Taking from an empty square leaves a lord carrying what they had")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an artefact, where nothing lies")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto crown = GetObject(ob_crown);
        lord->CarryObject(crown);

        WHEN("they try to take something")
        {
            auto taken = lord->Cmd_PickupObject();

            THEN("nothing is taken and they keep the artefact")
            {
                REQUIRE( taken == nullptr );
                REQUIRE( lord->Carrying() == crown );
            }
        }
    }
}

SCENARIO("Taking an artefact swaps it with the one the lord carries")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying one artefact where another lies")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto carried = GetObject(ob_crown);
        auto lying = GetObject(ob_other_crown);
        lord->CarryObject(carried);
        Lay(ob_other_crown, k_here);

        WHEN("they take the one on the ground")
        {
            lord->Cmd_PickupObject();

            THEN("they now carry that one, and the first is left there")
            {
                REQUIRE( lord->Carrying() == lying );
                REQUIRE( carried->Location() == k_here );
                REQUIRE_FALSE( carried->IsCarried() );
                REQUIRE( tme::mx->objectprocessor->FindObjectAtLocation(k_here) == carried );
            }
        }
    }
}

SCENARIO("A destroyed artefact is gone from the map")
{
    TMEStep::NewStory();

    GIVEN("an artefact lying on the ground")
    {
        LordWithEmptyHands(k_here);
        auto crown = GetObject(ob_crown);
        Lay(ob_crown, k_there);

        WHEN("it is destroyed")
        {
            tme::mx->objectprocessor->Destroy(crown);

            THEN("it is no longer found, and the square is no longer flagged")
            {
                REQUIRE( tme::mx->objectprocessor->FindObjectAtLocation(k_there) == nullptr );
                REQUIRE_FALSE( tme::mx->gamemap->HasObject(k_there) );
            }
        }
    }

    GIVEN("a lord carrying an artefact")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto crown = GetObject(ob_crown);
        lord->CarryObject(crown);

        WHEN("it is destroyed")
        {
            tme::mx->objectprocessor->Destroy(crown);

            THEN("they carry nothing")
            {
                REQUIRE( lord->Carrying() == nullptr );
            }
        }
    }
}

SCENARIO("Things that are not artefacts are not treated as artefacts")
{
    TMEStep::NewStory();

    THEN("the wolves are not")
    {
        REQUIRE_FALSE( GetObject("OB_WOLVES")->IsArtefact() );
    }
}

SCENARIO("A lord can only give what they carry to a lord with empty hands")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an artefact, and another lord on the same square")
    {
        auto lord = LordWithEmptyHands(k_here);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);
        auto crown = GetObject(ob_crown);
        lord->CarryObject(crown);
        other->CarryObject(nullptr);

        WHEN("they give it to the other lord")
        {
            auto result = static_cast<ddr_character*>(lord)->Cmd_Give(other);

            THEN("the other lord has it")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Carrying() == nullptr );
                REQUIRE( other->Carrying() == crown );
                REQUIRE( tme::mx->objectprocessor->WhoHasObject(crown) == other );
            }
        }

        WHEN("the other lord is already carrying something")
        {
            auto theirs = GetObject(ob_other_crown);
            other->CarryObject(theirs);
            auto result = static_cast<ddr_character*>(lord)->Cmd_Give(other);

            THEN("nothing changes")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Carrying() == crown );
                REQUIRE( other->Carrying() == theirs );
            }
        }

        WHEN("there is no one to give it to")
        {
            auto result = static_cast<ddr_character*>(lord)->Cmd_Give(nullptr);

            THEN("they keep it")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Carrying() == crown );
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
        auto artefact = GetObject(ob_crown);

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
