#include <cstddef>
#include <iostream>
#include <string>

#include "Team.hh"

/******************************************************************************
 * Constructor.
 *
 * @param tname Team name.
 * @param tid Team ID.
 *****************************************************************************/
Team::Team(std::string const& tname, std::size_t tid)
: tname(tname), tid(tid)
{
}

/******************************************************************************
 * Formatter.
 *
 * @param ostream
 * @param team
 *****************************************************************************/
std::ostream&
operator<<(std::ostream& ostream, Team const& team)
{
    ostream << team.tname << ':' << team.points;
    return ostream;
}
