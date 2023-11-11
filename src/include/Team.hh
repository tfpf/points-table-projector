#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_

#include <cstddef>
#include <string>

/******************************************************************************
 * Store information about a team.
 *****************************************************************************/
struct Team
{
    std::string const name;
    std::size_t const id;
    int points = 0;

    Team(std::string const& name, std::size_t id);
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_
