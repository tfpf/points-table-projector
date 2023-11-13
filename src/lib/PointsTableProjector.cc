#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "Fixture.hh"
#include "PointsTableProjector.hh"
#include "Team.hh"

#define THROW(exc, msg)                                                                                     \
    do {                                                                                                    \
        throw exc(std::string(__FILE__) + ':' + std::to_string(__LINE__) + " in " + __func__ + ". " + msg); \
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
void PointsTableProjector::parse(void)
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
            if (readline[4] == ' ') {
                favourite_tname = readline.substr(5, std::string::npos);
            } else if (readline[4] != '\0') {
                this->unknown_keyword_error(readline);
            }
            continue;
        }
        if (readline == "fixtures.completed" || readline == "fixtures.results") {
            if (!this->teams.empty()) {
                std::string message = "'" + readline + "' found in '" + this->fname;
                message += "' on line " + std::to_string(this->line_number);
                message += ", but one of 'fixtures.completed', 'fixtures.results' and 'fixtures.upcoming'";
                message += " has already been used previously.";
                THROW(std::invalid_argument, message);
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
            // Store these so that their results may be simulated.
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
        std::string message = "Favourite team not specified in '" + this->fname + "'.";
        THROW(std::runtime_error, message);
    }
    auto tname_tid_it = this->tname_tid.find(favourite_tname);
    if (tname_tid_it == this->tname_tid.end()) {
        std::string message = "No fixtures involving '" + favourite_tname + "' found in '" + this->fname + "'.";
        THROW(std::runtime_error, message);
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
void PointsTableProjector::unknown_keyword_error(std::string const& str)
{
    std::string message = "Unknown keyword '" + str + "' in '" + this->fname;
    message += "' on line " + std::to_string(this->line_number) + '.';
    THROW(std::invalid_argument, message);
}

/******************************************************************************
 * Store an integer in the specified variable.
 *
 * @param str String to parse.
 * @param intvar Variable to store the result in.
 *****************************************************************************/
void PointsTableProjector::parse_int(std::string const& str, int& intvar)
{
    try {
        intvar = std::stoi(str);
    } catch (std::exception& e) {
        std::string message = "Could not parse '" + str + "' as an integer in '" + this->fname;
        message += "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
    }
}

/******************************************************************************
 * Parse a string as a match between two teams.
 *
 * @param str String to parse.
 * @param update_points If `true`, update the points table. If `false`, store
 *     this match for later.
 *****************************************************************************/
void PointsTableProjector::parse_fixture(std::string const& str, bool update_points)
{
    std::size_t delim_idx = str.find_first_of(",=");
    if (delim_idx == std::string::npos) {
        std::string message = "Neither ',' nor '=' found in '" + this->fname;
        message += "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
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
 * Parse a string as an entry of the points table.
 *
 * @param str String to parse.
 *****************************************************************************/
void PointsTableProjector::parse_result(std::string const& str)
{
    std::size_t delim_idx = str.find(' ');
    if (delim_idx == std::string::npos) {
        std::string message = "Could not parse '" + str + "' as team name and points in '" + this->fname;
        message += "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
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
void PointsTableProjector::dump(void)
{
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
    int rank = 0;
    while (teams[rank++].tid != this->favourite_tid)
        ;
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
 * Find all possible results for our favourite team.
 *****************************************************************************/
void PointsTableProjector::solve(void)
{
    this->solve_(0);
}

/******************************************************************************
 * Find all possible results for our favourite team starting from the specified
 * fixture, and assuming they win all their fixtures.
 *
 * @param idx Fixture index.
 *****************************************************************************/
void PointsTableProjector::solve_(std::size_t idx)
{
    if (idx >= this->fixtures.size()) {
        this->dump();
        return;
    }
    Fixture& fixture = this->fixtures[idx];
    if (fixture.a.tid != this->favourite_tid) {
        fixture.ordered = false;
        fixture.a.points += this->points_lose;
        fixture.b.points += this->points_win;
        this->solve_(idx + 1);
        fixture.a.points -= this->points_lose;
        fixture.b.points -= this->points_win;
    }
    if (fixture.b.tid != this->favourite_tid) {
        fixture.ordered = true;
        fixture.a.points += this->points_win;
        fixture.b.points += this->points_lose;
        this->solve_(idx + 1);
        fixture.a.points -= this->points_win;
        fixture.b.points -= this->points_lose;
    }
}
