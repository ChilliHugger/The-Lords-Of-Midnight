//
//  routenode_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

SCENARIO("A Citadel route branch keeps a direction and a weight")
{
    citadel_entityfactory factory;
    std::unique_ptr<mxroutebranch> made( factory.CreateRouteBranch() );
    auto branch = dynamic_cast<citadel_routebranch*>(made.get());

    THEN("the Citadel makes its branches with them")
    {
        REQUIRE( branch != nullptr );
    }

    THEN("a new branch has no direction and no weight")
    {
        REQUIRE( branch->direction == DR_NONE );
        REQUIRE( branch->weight == 0 );
    }

    WHEN("the route is set")
    {
        branch->SetRoute(DR_NORTHEAST, 16);

        THEN("the branch gives back its direction and weight")
        {
            REQUIRE( branch->direction == DR_NORTHEAST );
            REQUIRE( branch->weight == 16 );
        }
    }
}

SCENARIO("A junction adds its branches through the scenario's factory")
{
    TMEStep::NewStory();

    auto from = GetEntity<mxroutenode>("RN_GreatOcean_Ulmor_0");
    mxroutenode other;

    GIVEN("a junction with no branches")
    {
        if ( from == nullptr )
            from = &other;
        from->ClearNodes();

        WHEN("a branch is added")
        {
            auto branch = from->AddNode(&other);

            THEN("it is the Citadel's own, and leads to the node")
            {
                REQUIRE( dynamic_cast<citadel_routebranch*>(branch) != nullptr );
                REQUIRE( branch->Node() == &other );
                REQUIRE( from->Nodes().size() == 1 );
                REQUIRE( from->Nodes()[0] == &other );
            }
        }

        WHEN("a branch to nowhere is added")
        {
            THEN("there is no branch")
            {
                REQUIRE( from->AddNode(nullptr) == nullptr );
                REQUIRE( from->Nodes().empty() );
            }
        }
    }
}
