#include <format>
#include <sstream>
#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>
#include <chrono>

#include "Fixture.hh"
#include "PointsTableProjector.hh"
#include "Team.hh"

#define THROW(exc, msg)                                                                                               \
    do                                                                                                                \
    {                                             \
        throw exc(std::string(__FILE__) + ':' + std::to_string(__LINE__) + " in " + __func__ + ". " + msg);           \
    } while (false)


#define CLOG(...) clog(__FILE__, __LINE__, __VA_ARGS__)

template<class...Args>
void clog(char const* _file_, int _line_, std::format_string<Args...> const fmt, Args&&... args)
{
    auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    std::clog << "\e[34m" << std::format("{:%F %T}", now) << "\e[m ";
    std::clog << "\e[94m" << _file_ << ':' << _line_ << "\e[m ";
    std::clog << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n';
}

/******************************************************************************
 * Constructor.
 *
 * @param fname Input file name.
 * @param raw_output Whether to show plain output (i.e. without colours).
 *****************************************************************************/
PointsTableProjector::PointsTableProjector(std::string const& fname, bool raw_output)
    : fname(fname)
    , line_number(0)
    , points_win(2)
    , points_lose(0)
    , points_other(1)
    , box_horizontal("─")
    , box_up_right("└")
    , box_vertical("│")
    , box_vertical_right("├")
    , inconsequential_begin("\e[90m")
    , inconsequential_end("\e[m")
{
    // Prevent reallocation in this member, because we are going to store
    // references to its elements in another member.
    this->teams.reserve(1024);
    this->parse();
    if (raw_output)
    {
        this->box_horizontal = this->box_up_right = this->box_vertical = this->box_vertical_right = " ";
        this->inconsequential_begin = this->inconsequential_end = "";
    }
}

/******************************************************************************
 * Read the contents of the input file and initialise all members accordingly.
 *****************************************************************************/
void
PointsTableProjector::parse(void)
{
    CLOG("Attempting to open {} for reading.", this->fname);
    if(!std::filesystem::is_regular_file(this->fname)){
        CLOG("Cannot open {}: not a regular file.", this->fname);
        throw std::runtime_error("invalid input");
    }
    std::ifstream fhandle(this->fname);
    if (!fhandle.good())
    {
        CLOG("Cannot open {} for reading.", this->fname);
        throw std::runtime_error("I/O error");
    }

    std::string line;
    while(std::getline(fhandle, line))
    {
        ++this->line_number;
        if(line.empty()){continue;}

        if(!line.starts_with('[') || !line.ends_with(']')) {
            CLOG("Expected a section in {}:{}. Found '{}'.", this->fname, this->line_number, line);
            throw std::runtime_error("parse failure");
        }
        line = line.substr(1, line.size() - 2);
        if(line == "team"){
            this->parse_favourite_tname(fhandle);
            continue;
        }
        if(line == "points"){
            this->parse_points(fhandle);
            continue;
        }
        if(line == "completed"){
            this->set_completed(fhandle);
            continue;
        }
    }
    throw std::runtime_error("");
}

void PointsTableProjector::parse_favourite_tname(std::ifstream& fhandle){
    CLOG("Parsing favourite team.");
    std::string line;
    while(std::getline(fhandle, line)){
        ++this->line_number;
        if(line.empty()){break;}
        this->favourite_tname = line;
    }
    if(this->favourite_tname.empty()){
        CLOG("Favourite team not specified in {}.", this->fname);
        throw std::runtime_error("parse failure");
    }
    CLOG("Set favourite team to {}.", this->favourite_tname);
}


void PointsTableProjector::parse_points(std::ifstream& fhandle){
    CLOG("Parsing points.");
    std::string line;
    while(std::getline(fhandle, line)){
        ++this->line_number;
        if(line.empty()){break;}
        std::istringstream line_stream(line);
        std::string outcome;
        line_stream >> outcome;
        if(outcome == "win"){
            line_stream >> this->points_win;
        } else if(outcome == "lose"){line_stream >> this->points_lose;}
        else if(outcome == "other"){line_stream >> this->points_other;}
        else {
            CLOG("Expected 'win', 'lose' or 'other', and an integer in {}:{}. Found '{}'.", this->fname, this->line_number, line);
            throw std::runtime_error("parse failure");
        }
        if(line_stream.fail()){
            CLOG("Expected an integer after 'win', 'lose' or 'other' in {}:{}. Found '{}'.", this->fname, this->line_number, line);
            throw std::runtime_error("parse failure");
        }
    }
    CLOG("Set points in case of a win to {}.", this->points_win);
    CLOG("Set points in case of a loss to {}.", this->points_lose);
    CLOG("Set points in other cases to {}.", this->points_other);
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
    try
    {
        intvar = std::stoi(str);
    }
    catch (std::exception& e)
    {
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
    if (delim_idx == std::string::npos)
    {
        THROW(std::invalid_argument,
            "Neither ',' nor '=' found in '" + this->fname + "' on line " + std::to_string(this->line_number) + '.');
    }
    std::size_t first_tid = this->reg(str.substr(0, delim_idx));
    std::size_t second_tid = this->reg(str.substr(delim_idx + 1, std::string::npos));
    if (update_points)
    {
        if (str[delim_idx] == '=')
        {
            this->teams[first_tid].points += this->points_other;
            this->teams[second_tid].points += this->points_other;
        }
        else
        {
            this->teams[first_tid].points += this->points_win;
            this->teams[second_tid].points += this->points_lose;
        }
    }
    else
    {
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
    if (delim_idx == std::string::npos)
    {
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
    if (tname_tid_it == this->tname_tid.end())
    {
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
    std::sort(teams.rbegin(), teams.rend(),
        [&](Team const& a, Team const& b)
        {
            return a.points < b.points || (a.points == b.points && b.tid == this->favourite_tid);
        });
    int rank = std::find_if(teams.begin(), teams.end(),
                   [&](Team const& t)
                   {
                       return t.tid == this->favourite_tid;
                   })
        - teams.begin() + 1;
    std::cout << rank << '\n';
    std::cout << this->box_vertical_right << this->box_horizontal << "fixtures.results\n";
    for (Team const& team : teams)
    {
        std::cout << this->box_vertical << "   " << team << '\n';
    }
    std::cout << this->box_up_right << this->box_horizontal << "fixtures.upcoming\n";
    for (Fixture const& fixture : this->fixtures)
    {
        if (fixture.inconsequential)
        {
            std::cout << "    " << this->inconsequential_begin << fixture << this->inconsequential_end << '\n';
        }
        else
        {
            std::cout << "    " << fixture << '\n';
        }
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
    std::vector<std::array<int, 2>> min_max(this->teams.size());
    for (Team const& team : this->teams)
    {
        min_max[team.tid][0] = min_max[team.tid][1] = team.points;
    }
    for (Fixture const& fixture : this->fixtures)
    {
        // Assume our favourite team always wins, so its minimum and maximum
        // points are the same.
        if (fixture.a.tid == this->favourite_tid || fixture.b.tid == this->favourite_tid)
        {
            min_max[this->favourite_tid][0] += this->points_win;
            min_max[this->favourite_tid][1] += this->points_win;
        }
        if (fixture.a.tid != this->favourite_tid)
        {
            min_max[fixture.a.tid][0] += this->points_lose;
            min_max[fixture.a.tid][1] += this->points_win;
        }
        if (fixture.b.tid != this->favourite_tid)
        {
            min_max[fixture.b.tid][0] += this->points_lose;
            min_max[fixture.b.tid][1] += this->points_win;
        }
    }

    // Which teams which will always finish with fewer/more points than our
    // favourite team?
    for (Team& team : this->teams)
    {
        team.inconsequential = min_max[team.tid][1] < min_max[this->favourite_tid][0]
            || min_max[team.tid][0] > min_max[this->favourite_tid][1];
    }

    // Which fixtures are between such teams?
    for (Fixture& fixture : this->fixtures)
    {
        fixture.inconsequential = fixture.a.inconsequential && fixture.b.inconsequential;
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
    if (idx >= this->fixtures.size())
    {
        this->dump();
        return;
    }

    static std::random_device rdev;
    static std::mt19937 mtwist(69899617);
    static auto bgen = [&]()
    {
        return static_cast<bool>(mtwist() & 1);
    };

    // If the outcome of this fixture does not matter, pick a winner randomly.
    Fixture& fixture = this->fixtures[idx];
    if (fixture.inconsequential)
    {
        fixture.ordered = bgen();
        if (!fixture.ordered)
        {
            this->solve__(idx, fixture.b, fixture.a);
        }
        else
        {
            this->solve__(idx, fixture.a, fixture.b);
        }
        return;
    }

    // Assume that our favourite team always wins.
    if (fixture.a.tid != this->favourite_tid)
    {
        fixture.ordered = false;
        this->solve__(idx, fixture.b, fixture.a);
    }
    if (fixture.b.tid != this->favourite_tid)
    {
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
