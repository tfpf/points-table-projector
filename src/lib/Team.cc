#include <cstddef>
#include <string>

#include "Team.hh"

/******************************************************************************
 * Constructor.
 *
 * @param name
 *****************************************************************************/
Team::Team(std::string const& name, std::size_t id)
: name(name), id(id)
{
}
