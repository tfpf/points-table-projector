#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "PointsTableProjector.hh"

/******************************************************************************
 * Main function.
 *
 * @param argc
 * @param argv
 *****************************************************************************/
int
main(int const argc, char const* argv[])
{
    // It's easier to parse using C functions, since the argument vector
    // contains C strings. Also, write errors to the C error stream, since that
    // is where logging messages go in the rest of this project.
    char const* fname = NULL;
    bool raw_output = false;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-')
        {
            if (fname != NULL)
            {
                std::fprintf(stderr, "Extra argument: '%s'.\n", argv[i]);
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
        std::fprintf(stderr, "Unknown option: '%s'.\n", argv[i]);
        return EXIT_FAILURE;
    }
    if (fname == NULL)
    {
        std::fprintf(stderr, "Input file not specified.\n");
        return EXIT_FAILURE;
    }
    PointsTableProjector projector(fname, raw_output);
    projector.solve();
}
