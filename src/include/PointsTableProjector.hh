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
    PointsTableProjector(std::string const& fname, bool raw_output);
    void dump(void);
    void solve(void);

private:
    void parse(void);
    void parse_favourite_team(std::ifstream& fhandle);
    void parse_points(std::ifstream& fhandle);
    void parse_points_table(std::ifstream& fhandle);
    void parse_fixture(std::ifstream& fhandle, bool completed);
    std::size_t tname_to_tid(std::string const& tname);
    void solve_(std::size_t idx);
    void solve__(std::size_t idx, Team& winner, Team& loser);

private:
    std::string const fname;
    int line_number;
    int points_win;
    int points_loss;
    int points_other;
    std::string favourite_tname;
    std::size_t favourite_tid;
    // Mapping between team names and team IDs.
    std::unordered_map<std::string, std::size_t const> tname_tid;
    std::vector<Team> teams;
    // Upcoming fixtures. (Completed fixtures are not stored.)
    std::vector<Fixture> upcoming_fixtures;
    // Output decorations.
    char const* box_horizontal;
    char const* box_up_right;
    char const* box_vertical;
    char const* box_vertical_right;
    char const* inconsequential_begin;
    char const* inconsequential_end;
    char const* section_begin;
    char const* section_end;
};

#endif  // TFPF_POINTS_TABLE_PROJECTOR_SRC_INCLUDE_POINTSTABLEPROJECTOR_HH_
