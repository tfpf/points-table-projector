#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "PointsTableProjector.hh"

/******************************************************************************
 * Main function.
 *****************************************************************************/
int
main(int const argc, char const* argv[])
{
    // It's easier to parse using C functions, since the argument vector
    // contains C strings.
    char const* fname = NULL;
    bool raw_output = false;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-')
        {
            if (fname != NULL)
            {
                std::clog << "Extra argument: '" << argv[i] << "'.\n";
                return EXIT_FAILURE;
            }
            fname = argv[i];
            continue;
        }
        if (std::strcmp(argv[i], "-r") == 0)
        {
            raw_output = true;
            continue;
        }
        std::clog << "Unknown option: '" << argv[i] << "'.\n";
        return EXIT_FAILURE;
    }
    if (fname == NULL)
    {
        std::clog << "Input file not specified.\n";
        return EXIT_FAILURE;
    }
    PointsTableProjector projector(fname, raw_output);
    projector.solve();
}
