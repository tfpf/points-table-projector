#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_

#include "Team.hh"

struct Fixture
{
    Team& a;
    Team& b;

    Fixture(Team& a, Team& b);
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_
