#ifndef TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_POINTSTABLEPROJECTOR_HH_
#define TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_POINTSTABLEPROJECTOR_HH_

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "Fixture.hh"
#include "Team.hh"

class PointsTableProjector
{
    public:
    PointsTableProjector(std::string const& fname);
    void debug(void);
    void solve(void);

    private:
    void parse(void);
    void parse_int(char const *str, int& var);
    void parse_fixture(std::string const& str, bool update_points);
    std::size_t reg(std::string const& tname);
    void solve_(std::size_t idx);

    private:
    std::string const fname;
    int line_number = 0;
    int points_win = 2;
    int points_lose = 0;
    int points_other = 1;
    std::size_t my_tid = 0;
    std::unordered_map<std::string, std::size_t const> tname_tid;
    std::vector<Team> teams;
    std::vector<Fixture> fixtures;
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_POINTSTABLEPROJECTOR_HH_
