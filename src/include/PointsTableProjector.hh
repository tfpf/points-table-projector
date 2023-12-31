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
    void dump(void);
    void solve(void);

private:
    void parse(void);
    void unknown_keyword_error(std::string const& str);
    void parse_int(std::string const& str, int& var);
    void parse_fixture(std::string const& str, bool update_points);
    void parse_result(std::string const& str);
    std::size_t reg(std::string const& tname);
    void solve_(std::size_t idx);
    void solve__(std::size_t idx, Team& winner, Team& loser);

private:
    std::string const fname;
    int line_number;
    int points_win;
    int points_lose;
    int points_other;
    std::size_t favourite_tid;
    // Mapping between team names and team IDs.
    std::unordered_map<std::string, std::size_t const> tname_tid;
    std::vector<Team> teams;
    // Upcoming fixtures. (Completed fixtures are not stored.)
    std::vector<Fixture> fixtures;
    // Teams whose performances do not affect our favourite team's position.
    std::vector<bool> inconsequential;
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_POINTSTABLEPROJECTOR_HH_
