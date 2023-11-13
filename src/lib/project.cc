#include <cstdlib>
#include <iostream>

#include "PointsTableProjector.hh"

/******************************************************************************
 * Main function.
 *****************************************************************************/
int main(int const argc, char const* argv[])
{
    if (argc < 2) {
        std::clog << "Input file not specified.\n";
        return EXIT_FAILURE;
    }
    PointsTableProjector projector(argv[1]);
    projector.solve();
}
