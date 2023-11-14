#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_

#include <cstddef>
#include <iostream>
#include <string>

/******************************************************************************
 * Store information about a team.
 *****************************************************************************/
struct Team {
    std::string tname;
    std::size_t tid;
    int points = 0;

    Team(std::string const& tname, std::size_t tid);
};

std::ostream& operator<<(std::ostream& ostream, Team const& team);

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_
