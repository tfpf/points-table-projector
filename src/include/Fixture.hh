#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_

#include <iostream>

#include "Team.hh"

/******************************************************************************
 * Store information about a fixture.
 *****************************************************************************/
struct Fixture {
    // These two members do not belong to this object. Their lifetime (and the
    // lifetime of this object) is entirely controlled by the class which
    // instantiates them.
    Team& a;
    Team& b;
    // Whether the first team won the fixture.
    bool ordered = true;

    Fixture(Team& a, Team& b);
};

std::ostream& operator<<(std::ostream& ostream, Fixture const& fixture);

#endif // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_FIXTURE_HH_
