#include <cstddef>
#include <string>

#include "Team.hh"

/******************************************************************************
 * Constructor.
 *
 * @param tname Team name.
 * @param it Team ID.
 *****************************************************************************/
Team::Team(std::string const& tname, std::size_t tid)
: tname(tname), tid(tid)
{
}
