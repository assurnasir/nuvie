#include "nuvieDefs.h"
#include "Actor.h"
#include "Path.h"
#include "ActorPathFinder.h"

ActorPathFinder::ActorPathFinder(Actor *a, MapCoord g)
                   : PathFinder(a->get_location(), g), actor(a)
{

}

ActorPathFinder::~ActorPathFinder()
{

}

bool ActorPathFinder::get_next_move(MapCoord &step)
{
    MapCoord rel_step;
    if(have_path())
    {
        step = search->get_first_step();
        return check_loc(step);
    }

    get_closest_dir(rel_step);
    if(check_dir(loc, rel_step))
    {
        step = loc.abs_coords(rel_step.sx, rel_step.sy);
        return true;
    }
    if(search_towards_target(goal, rel_step))
    {
        step = loc.abs_coords(rel_step.sx, rel_step.sy);
        return true;
    }

    if(find_path())
    {
        step = search->get_first_step();
        return check_loc(step);
    }
    return false;
}

/* Get relative direction from Loc to Goal and place in Rel_step. */
void ActorPathFinder::get_closest_dir(MapCoord &rel_step)
{
    rel_step.sx = clamp(goal.x - loc.x, -1, 1);
    rel_step.sy = clamp(goal.y - loc.y, -1, 1);
    rel_step.z = loc.z;

    uint16 dx = loc.xdistance(goal), dy = loc.ydistance(goal);
    if(dx > dy) rel_step.sy = 0;
    else if(dx < dy) rel_step.sx = 0;
}

bool ActorPathFinder::check_loc(const MapCoord &loc)
{
    return actor->check_move(loc.x, loc.y, loc.z);
}

/* Find a move from actor to g, starting with rel_step. Replace
 * rel_step with the result. */
bool ActorPathFinder::search_towards_target(const MapCoord &g, MapCoord &rel_step)
{
    MapCoord loc = actor->get_location();
    MapCoord ccw_rel_step = rel_step, cw_rel_step = rel_step;
    if(check_dir(loc, rel_step)) // check original direction
        return true;
    bool try_ccw = check_dir_and_distance(loc, g, ccw_rel_step, -1); // check adjacent directions
    bool try_cw = check_dir_and_distance(loc, g, cw_rel_step, 1);
    if(!try_ccw) try_ccw = check_dir_and_distance(loc, g, ccw_rel_step, -2); // check perpendicular directions
    if(!try_cw) try_cw = check_dir_and_distance(loc, g, cw_rel_step, 2);
    if(!try_ccw && !try_cw) 
        return false;
    rel_step = ccw_rel_step;
    if(!try_ccw) rel_step = cw_rel_step;
    else if(!try_cw) rel_step = ccw_rel_step;
    else // both valid, use closest
    {
        MapCoord ccw_step = loc.abs_coords(ccw_rel_step.sx,ccw_rel_step.sy);
        MapCoord cw_step = loc.abs_coords(cw_rel_step.sx,cw_rel_step.sy);
        MapCoord target(g);
        if(cw_step.distance(target) < ccw_step.distance(target))
            rel_step = cw_rel_step;
    }
    return true;
}

// check rotated dir, and copy results to rel_step if neighbor is passable
bool ActorPathFinder::check_dir_and_distance(MapCoord loc, MapCoord g, MapCoord &rel_step, sint8 rotate)
{
    MapCoord rel_step_2 = rel_step;
    if(check_dir(loc, rel_step_2, rotate))
    {
        MapCoord neighbor = loc.abs_coords(rel_step_2.sx, rel_step_2.sy);
        if(neighbor.distance(g) <= loc.distance(g))
        {
            rel_step = rel_step_2;
            return true;
        }
    }
    return false;
}

// new direction is copied to rel if true
bool ActorPathFinder::check_dir(const MapCoord &loc, MapCoord &rel, sint8 rot)
{
    sint8 xdir = rel.sx, ydir = rel.sy;
    get_adjacent_dir(xdir, ydir, rot);
    MapCoord new_loc = MapCoord(loc).abs_coords(xdir, ydir);
    if(check_loc(new_loc))
    {
        rel.sx = xdir;
        rel.sy = ydir;
        return true;
    }
    return false;
}

void ActorPathFinder::actor_moved()
{
    update_location();
    // pop step
    if(have_path())
        search->remove_first_step();
}

void ActorPathFinder::set_actor(Actor *a)
{
    actor = a;
}

bool ActorPathFinder::update_location()
{
    if(!actor)
        return false;
    actor->get_location(&loc.x, &loc.y, &loc.z);
    return true;
}
