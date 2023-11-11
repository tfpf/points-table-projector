#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_

#include "Team.hh"

/******************************************************************************
 * Store information about a fixture. The members are two objects which do not
 * belong to this object. Their lifetime (and indeed the lifetime of this
 * object) is entirely controlled by the class which instantiates them.
 *****************************************************************************/
struct Fixture
{
    Team& a;
    Team& b;

    Fixture(Team& a, Team& b);
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_
