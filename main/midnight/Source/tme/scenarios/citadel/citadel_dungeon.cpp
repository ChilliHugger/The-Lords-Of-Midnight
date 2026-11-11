/*
 * FILE:    citadel_dungeon.cpp
 *
 * PROJECT: citadel
 *
 * PURPOSE: The passages under a citadel, built from a seed.
 *
 */

#include "../../baseinc/tme_internal.h"
#include "scenario_citadel_internal.h"

#include <queue>

#if defined(_CITADEL_)
namespace tme {

void citadel_dungeon::Generate ( u32 newseed )
{
    seed = newseed;
    std::fill(std::begin(exits), std::end(exits), (u8)0);
    rooms.clear();

    randomno rng;
    rng.seed(seed);

    std::vector<mxgridref> trail { Entrance() };
    u32 dug = 1;
    while ( !trail.empty() && dug < PASSAGES ) {
        auto here = trail.back();

        mxdir_t open[4];
        s32 count = 0;
        for ( auto dir : { DR_NORTH, DR_EAST, DR_SOUTH, DR_WEST } ) {
            auto next = Neighbour(here, dir);
            if ( IsOnGrid(next) && exits[Index(next)] == 0 && next != Entrance() )
                open[count++] = dir;
        }

        if ( count == 0 ) {
            trail.pop_back();
            continue;
        }

        auto dir = open[rng.get(count - 1)];
        auto next = Neighbour(here, dir);
        exits[Index(here)] |= ExitBit(dir);
        exits[Index(next)] |= ExitBit((mxdir_t)((dir + 4) % 8));
        trail.push_back(next);
        dug++;
    }

    for ( s32 y = 0; y < SIZE; y++ ) {
        for ( s32 x = 0; x < SIZE; x++ ) {
            mxgridref cell(x, y);
            auto ways = exits[Index(cell)];
            if ( ways != 0 && ( ways & ( ways - 1 ) ) == 0 && cell != Entrance() )
                rooms.push_back(cell);
        }
    }

    for ( s32 ii = (s32)rooms.size() - 1; ii > 0; ii-- )
        std::swap(rooms[ii], rooms[rng.get(ii)]);
}

void citadel_dungeon::Serialize ( archive& ar )
{
    if ( ar.IsStoring() ) {
        ar << seed;
    } else {
        ar >> seed;
        Generate(seed);
    }
}

bool citadel_dungeon::HasPassage ( mxgridref from, mxdir_t dir ) const
{
    return !IS_DIAGONAL(dir) && IsOnGrid(from) && ( exits[Index(from)] & ExitBit(dir) ) != 0;
}

bool citadel_dungeon::IsWayOut ( mxgridref from, mxdir_t dir ) const
{
    return from == Entrance() && dir == DR_SOUTH;
}

mxgridref citadel_dungeon::Entrance () const
{
    return mxgridref(SIZE / 2, SIZE - 1);
}

mxgridref citadel_dungeon::Start () const
{
    return rooms.empty() ? Entrance() : rooms.front();
}

mxgridref citadel_dungeon::CellOf ( const mxcharacter* hostage ) const
{
    if ( rooms.size() < 2 )
        return Start();
    return rooms[1 + hostage->Id() % ( rooms.size() - 1 )];
}

mxgridref citadel_dungeon::Neighbour ( mxgridref from, mxdir_t dir ) const
{
    switch ( dir ) {
        case DR_NORTH:  from.y--; break;
        case DR_EAST:   from.x++; break;
        case DR_SOUTH:  from.y++; break;
        case DR_WEST:   from.x--; break;
        default:        break;
    }
    return from;
}

mxdir_t citadel_dungeon::StepTowards ( mxgridref from, mxgridref to ) const
{
    if ( !IsOnGrid(from) || !IsOnGrid(to) )
        return DR_NONE;

    std::vector<s32> steps(SIZE * SIZE, -1);
    std::queue<mxgridref> queue;
    steps[Index(to)] = 0;
    queue.push(to);
    while ( !queue.empty() ) {
        auto here = queue.front();
        queue.pop();
        for ( auto dir : { DR_NORTH, DR_EAST, DR_SOUTH, DR_WEST } ) {
            auto next = Neighbour(here, dir);
            if ( HasPassage(here, dir) && steps[Index(next)] < 0 ) {
                steps[Index(next)] = steps[Index(here)] + 1;
                queue.push(next);
            }
        }
    }

    for ( auto dir : { DR_NORTH, DR_EAST, DR_SOUTH, DR_WEST } ) {
        if ( HasPassage(from, dir) && steps[Index(Neighbour(from, dir))] == steps[Index(from)] - 1 )
            return dir;
    }
    return DR_NONE;
}

bool citadel_dungeon::IsOnGrid ( mxgridref cell ) const
{
    return cell.x >= 0 && cell.y >= 0 && cell.x < SIZE && cell.y < SIZE;
}

u32 citadel_dungeon::Index ( mxgridref cell ) const
{
    return (u32)( cell.y * SIZE + cell.x );
}

u8 citadel_dungeon::ExitBit ( mxdir_t dir ) const
{
    return (u8)( 1 << ( dir / 2 ) );
}

}
// namespace tme
#endif
