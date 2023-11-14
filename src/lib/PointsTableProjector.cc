#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>

#include "Fixture.hh"
#include "PointsTableProjector.hh"
#include "Team.hh"

#define THROW(exc, msg)                                                                                               \
    do {                                                                                                              \
        throw exc(std::string(__FILE__) + ':' + std::to_string(__LINE__) + " in " + __func__ + ". " + msg);           \
    } while (false)

/******************************************************************************
 * Constructor.
 *
 * @param fname Input file name.
 *****************************************************************************/
PointsTableProjector::PointsTableProjector(std::string const& fname)
    : fname(fname)
{
    // Prevent reallocation in this member, because we are going to store
    // references to its elements in another member.
    this->teams.reserve(1024);
    this->parse();
}

/******************************************************************************
 * Read the contents of the input file and initialise all members accordingly.
 *****************************************************************************/
void
PointsTableProjector::parse(void)
{
    std::ifstream fhandle(this->fname);
    if (!fhandle.good() || !std::filesystem::is_regular_file(this->fname)) {
        THROW(std::runtime_error, "Could not read '" + this->fname + "'.");
    }
    std::string favourite_tname;
    while (true) {
        std::string readline;
        ++this->line_number;
        if (!std::getline(fhandle, readline)) {
            break;
        }
        if (readline.empty()) {
            continue;
        }
        // C++11 and newer strings are guaranteed to end with a null character,
        // so it is safe to take the address of a character and treat it as a C
        // string, as seen below.
        if (readline.rfind("points.win", 0) == 0) {
            this->parse_int(&readline[10], this->points_win);
            continue;
        }
        if (readline.rfind("points.lose", 0) == 0) {
            this->parse_int(&readline[11], this->points_lose);
            continue;
        }
        if (readline.rfind("points.other", 0) == 0) {
            this->parse_int(&readline[12], this->points_other);
            continue;
        }
        if (readline.rfind("team", 0) == 0) {
            // If this word is followed by nothing or a space, do not error
            // out. Otherwise, identify it as the error it is.
            if (readline[4] == ' ') {
                favourite_tname = readline.substr(5, std::string::npos);
            } else if (readline[4] != '\0') {
                this->unknown_keyword_error(readline);
            }
            continue;
        }
        if (readline == "fixtures.completed" || readline == "fixtures.results") {
            if (!this->teams.empty()) {
                THROW(std::invalid_argument,
                    "'" + readline + "' found in '" + this->fname + "' on line " + std::to_string(this->line_number)
                        + ", but one of 'fixtures.completed', 'fixtures.results' and 'fixtures.upcoming' has already "
                          "been used previously.");
            }
            bool completed_or_results = readline == "fixtures.completed";
            while (std::getline(fhandle, readline) && !readline.empty()) {
                ++this->line_number;
                completed_or_results ? this->parse_fixture(readline, true) : this->parse_result(readline);
            }
            ++this->line_number;
            continue;
        }
        if (readline == "fixtures.upcoming") {
            while (std::getline(fhandle, readline) && !readline.empty()) {
                ++this->line_number;
                this->parse_fixture(readline, false);
            }
            ++this->line_number;
            continue;
        }
        this->unknown_keyword_error(readline);
    }
    if (favourite_tname.empty()) {
        THROW(std::runtime_error, "Favourite team not specified in '" + this->fname + "'.");
    }
    auto tname_tid_it = this->tname_tid.find(favourite_tname);
    if (tname_tid_it == this->tname_tid.end()) {
        THROW(std::runtime_error, "No fixtures involving '" + favourite_tname + "' found in '" + this->fname + "'.");
    }
    this->favourite_tid = tname_tid_it->second;
    if (this->fixtures.empty()) {
        THROW(std::runtime_error, "'fixtures.upcoming' empty or not specified in '" + this->fname + "'.");
    }
}

/******************************************************************************
 * Generic error.
 *
 * @param str String which led to the error.
 *****************************************************************************/
void
PointsTableProjector::unknown_keyword_error(std::string const& str)
{
    THROW(std::invalid_argument,
        "Unknown keyword '" + str + "' in '" + this->fname + "' on line " + std::to_string(this->line_number) + '.');
}

/******************************************************************************
 * Parse a string as an integer. Store it in the specified variable.
 *
 * @param str String to parse. Expected to be numeric.
 * @param intvar Variable to store the result in.
 *****************************************************************************/
void
PointsTableProjector::parse_int(std::string const& str, int& intvar)
{
    try {
        intvar = std::stoi(str);
    } catch (std::exception& e) {
        THROW(std::invalid_argument,
            "Could not parse '" + str + "' as an integer in '" + this->fname + "' on line "
                + std::to_string(this->line_number) + '.');
    }
}

/******************************************************************************
 * Parse a string as a match between two teams, registering them if necessary.
 *
 * @param str String to parse. Expected to be two comma- or equals-separated
 *     words.
 * @param update_points Whether this match has already been played. If `true`,
 *     update the points table. If `false`, store this match for later.
 *****************************************************************************/
void
PointsTableProjector::parse_fixture(std::string const& str, bool update_points)
{
    std::size_t delim_idx = str.find_first_of(",=");
    if (delim_idx == std::string::npos) {
        THROW(std::invalid_argument,
            "Neither ',' nor '=' found in '" + this->fname + "' on line " + std::to_string(this->line_number) + '.');
    }
    std::size_t first_tid = this->reg(str.substr(0, delim_idx));
    std::size_t second_tid = this->reg(str.substr(delim_idx + 1, std::string::npos));
    if (update_points) {
        if (str[delim_idx] == '=') {
            this->teams[first_tid].points += this->points_other;
            this->teams[second_tid].points += this->points_other;
        } else {
            this->teams[first_tid].points += this->points_win;
            this->teams[second_tid].points += this->points_lose;
        }
    } else {
        this->fixtures.emplace_back(Fixture(this->teams[first_tid], this->teams[second_tid]));
    }
}

/******************************************************************************
 * Parse a string as an entry of the points table. Update the points table
 * accordingly.
 *
 * @param str String to parse. Expected to word and an integer separated by a
 *     space.
 *****************************************************************************/
void
PointsTableProjector::parse_result(std::string const& str)
{
    std::size_t delim_idx = str.find(' ');
    if (delim_idx == std::string::npos) {
        THROW(std::invalid_argument,
            "Could not parse '" + str + "' as team name and points in '" + this->fname + "' on line "
                + std::to_string(this->line_number) + '.');
    }
    std::size_t tid = this->reg(str.substr(0, delim_idx));
    this->parse_int(&str[delim_idx + 1], this->teams[tid].points);
}

/******************************************************************************
 * Register a team by generating an ID for it if it isn't already registered.
 * The ID is the index at which this team is inserted.
 *
 * @param tname Team name.
 *
 * @return Team ID.
 *****************************************************************************/
std::size_t
PointsTableProjector::reg(std::string const& tname)
{
    auto tname_tid_it = this->tname_tid.find(tname);
    if (tname_tid_it == this->tname_tid.end()) {
        std::size_t tid = this->tname_tid.size();
        this->tname_tid.insert({ tname, tid });
        this->teams.emplace_back(Team(tname, tid));
        return tid;
    }
    return tname_tid_it->second;
}

/******************************************************************************
 * Display some members in readable form.
 *****************************************************************************/
void
PointsTableProjector::dump(void)
{
    // Arrange the teams in decreasing order of points. If two have the same
    // points, place our favourite team at the lower index.
    std::vector<Team> teams(this->teams);
    std::sort(teams.rbegin(), teams.rend(), [&](Team const& a, Team const& b) {
        if (a.points > b.points) {
            return false;
        }
        if (a.points < b.points || b.tid == this->favourite_tid) {
            return true;
        }
        return false;
    });
    int rank = std::find_if(teams.begin(), teams.end(),
                   [&](Team const& t) {
                       return t.tid == this->favourite_tid;
                   })
        - teams.begin() + 1;
    std::cout << rank << '\n';
    std::cout << "  fixtures.results\n";
    for (Team const& team : teams) {
        std::cout << "    " << team << '\n';
    }
    std::cout << "  fixtures.upcoming\n";
    for (Fixture const& fixture : this->fixtures) {
        std::cout << "    " << fixture << '\n';
    }
}

/******************************************************************************
 * Find all possible results for our favourite team (assuming they win all
 * their fixtures).
 *****************************************************************************/
void
PointsTableProjector::solve(void)
{
    // Calculate the minimum and maximum points each team can earn.
    std::vector<int[2]> min_max(this->teams.size());
    for (Team const& team : this->teams) {
        min_max[team.tid][0] = min_max[team.tid][1] = team.points;
    }
    for (Fixture const& fixture : this->fixtures) {
        // Assume our favourite team always wins, so its minimum and maximum
        // points are the same.
        if (fixture.a.tid == this->favourite_tid || fixture.b.tid == this->favourite_tid) {
            min_max[this->favourite_tid][0] += this->points_win;
            min_max[this->favourite_tid][1] += this->points_win;
        }
        if (fixture.a.tid != this->favourite_tid) {
            min_max[fixture.a.tid][0] += this->points_lose;
            min_max[fixture.a.tid][1] += this->points_win;
        }
        if (fixture.b.tid != this->favourite_tid) {
            min_max[fixture.b.tid][0] += this->points_lose;
            min_max[fixture.b.tid][1] += this->points_win;
        }
    }

    // Which teams which will always finish with fewer/more points than our
    // favourite team?
    this->inconsequential.resize(this->teams.size(), false);
    for (Team const& team : this->teams) {
        this->inconsequential[team.tid] = min_max[team.tid][1] < min_max[this->favourite_tid][0]
            || min_max[team.tid][0] > min_max[this->favourite_tid][1];
    }

    this->solve_(0);
}

/******************************************************************************
 * Find all possible results for our favourite team (assuming they win all
 * their fixtures) starting from the specified fixture.
 *
 * @param idx Fixture index.
 *****************************************************************************/
void
PointsTableProjector::solve_(std::size_t idx)
{
    if (idx >= this->fixtures.size()) {
        this->dump();
        return;
    }

    static std::random_device rdev;
    static std::mt19937 mtwist(rdev());
    static auto bgen = [&]() {
        return static_cast<bool>(mtwist() & 1);
    };

    // If the outcome of this fixture does not matter, pick a winner randomly.
    Fixture& fixture = this->fixtures[idx];
    if (this->inconsequential[fixture.a.tid] && this->inconsequential[fixture.b.tid]) {
        fixture.ordered = bgen();
        if (!fixture.ordered) {
            this->solve__(idx, fixture.b, fixture.a);
        } else {
            this->solve__(idx, fixture.a, fixture.b);
        }
        return;
    }

    // Assume that our favourite team always wins.
    if (fixture.a.tid != this->favourite_tid) {
        fixture.ordered = false;
        this->solve__(idx, fixture.b, fixture.a);
    }
    if (fixture.b.tid != this->favourite_tid) {
        fixture.ordered = true;
        this->solve__(idx, fixture.a, fixture.b);
    }
}

/******************************************************************************
 * Recursion helper. Simulate the result of a fixture, recurse and unsimulate.
 *
 * @param idx Fixture index.
 * @param winner
 * @param loser
 *****************************************************************************/
void
PointsTableProjector::solve__(std::size_t idx, Team& winner, Team& loser)
{
    winner.points += this->points_win;
    loser.points += this->points_lose;
    this->solve_(idx + 1);
    loser.points -= this->points_lose;
    winner.points -= this->points_win;
}
