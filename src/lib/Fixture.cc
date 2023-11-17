#include <iostream>

#include "Fixture.hh"
#include "Team.hh"

/******************************************************************************
 * Constructor.
 *
 * @param a
 * @param b
 *****************************************************************************/
Fixture::Fixture(Team& a, Team& b)
    : a(a)
    , b(b)
{
}

/******************************************************************************
 * Formatter.
 *
 * @param ostream
 * @param fixture
 *****************************************************************************/
std::ostream&
operator<<(std::ostream& ostream, Fixture const& fixture)
{
    if (fixture.ordered)
    {
        ostream << fixture.a.tname << ',' << fixture.b.tname;
    }
    else
    {
        ostream << fixture.b.tname << ',' << fixture.a.tname;
    }
    return ostream;
}
