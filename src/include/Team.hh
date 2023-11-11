#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_

struct Team
{
    std::string const name;
    int points = 0;

    Team(std::string const& name);
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_TEAM_HH_
