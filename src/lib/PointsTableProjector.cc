#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Fixture.hh"
#include "PointsTableProjector.hh"
#include "Team.hh"

#define CLOG(...) clog(__FILE__, __LINE__, __VA_ARGS__)

template <class... Args>
void
clog(char const* _file_, int _line_, std::format_string<Args...> const fmt, Args&&... args)
{
    // auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    // std::clog << "\e[34m" << std::format("{:%F %T}", now) << "\e[m ";
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
    , points_loss(0)
    , points_other(1)
    , box_horizontal("─")
    , box_up_right("└")
    , box_vertical("│")
    , box_vertical_right("├")
    , inconsequential_begin("\e[90m")
    , inconsequential_end("\e[m")
    , section_begin("[\e[92m")
    , section_end("\e[m]")
{
    // Prevent reallocation in this member, because we are going to store
    // references to its elements in another member.
    this->teams.reserve(1024);
    this->parse();
    if (raw_output)
    {
        this->box_horizontal = this->box_up_right = this->box_vertical = this->box_vertical_right = " ";
        this->inconsequential_begin = this->inconsequential_end = "";
        this->section_begin = "[";
        this->section_end = "]";
    }
}

/******************************************************************************
 * Read the contents of the input file and initialise all members accordingly.
 *****************************************************************************/
void
PointsTableProjector::parse(void)
{
    CLOG("Attempting to open {} for reading.", this->fname);
    if (!std::filesystem::is_regular_file(this->fname))
    {
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
    while (std::getline(fhandle, line))
    {
        ++this->line_number;
        if (line.empty())
        {
            continue;
        }
        if (!line.starts_with('[') || !line.ends_with(']'))
        {
            CLOG("Expected a section in {}:{}. Found '{}'.", this->fname, this->line_number, line);
            throw std::runtime_error("parse failure");
        }
        line = line.substr(1, line.size() - 2);
        if (line == "team")
        {
            this->parse_favourite_team(fhandle);
            continue;
        }
        if (line == "points")
        {
            this->parse_points(fhandle);
            continue;
        }
        if (line == "table")
        {
            if (!this->teams.empty())
            {
                CLOG("Cannot specify points table in {}:{} because points table or completed fixtures have already "
                     "been specified.",
                    this->fname, this->line_number);
                throw std::runtime_error("parse failure");
            }
            this->parse_points_table(fhandle);
            continue;
        }
        if (line == "completed")
        {
            if (!this->teams.empty())
            {
                CLOG("Cannot specify completed fixtures in {}:{} because points table or completed fixtures have "
                     "already been specified.",
                    this->fname, this->line_number);
                throw std::runtime_error("parse failure");
            }
            this->parse_fixture(fhandle, true);
            continue;
        }
        if (line == "upcoming")
        {
            this->parse_fixture(fhandle, false);
            continue;
        }
        CLOG("Expected a known section in {}:{}. Found '{}'.", this->fname, this->line_number, line);
        throw std::runtime_error("parse failure");
    }

    if (this->favourite_tname.empty())
    {
        CLOG("Favourite team not specified in {}.", this->fname);
        throw std::runtime_error("parse failure");
    }
    this->favourite_tid = this->tname_to_tid(this->favourite_tname);
    if (this->upcoming_fixtures.empty())
    {
        CLOG("Upcoming fixtures not specified in {}.", this->fname);
        throw std::runtime_error("parse failure");
    }
}

/******************************************************************************
 * Note the favourite team.
 *
 * @param fhandle Stream to read from.
 *****************************************************************************/
void
PointsTableProjector::parse_favourite_team(std::ifstream& fhandle)
{
    CLOG("Parsing favourite team.");
    std::string line;
    while (std::getline(fhandle, line))
    {
        ++this->line_number;
        if (line.empty())
        {
            break;
        }
        this->favourite_tname = line;
    }
    CLOG("Set favourite team to '{}'.", this->favourite_tname);
}

/******************************************************************************
 * Note the points given in different situations.
 *
 * @param fhandle Stream to read from.
 *****************************************************************************/
void
PointsTableProjector::parse_points(std::ifstream& fhandle)
{
    CLOG("Parsing points.");
    std::string line;
    while (std::getline(fhandle, line))
    {
        ++this->line_number;
        if (line.empty())
        {
            break;
        }
        std::istringstream line_stream(line);
        std::string outcome;
        line_stream >> outcome;
        if (outcome == "win")
        {
            line_stream >> this->points_win;
        }
        else if (outcome == "loss")
        {
            line_stream >> this->points_loss;
        }
        else if (outcome == "other")
        {
            line_stream >> this->points_other;
        }
        else
        {
            CLOG("Expected 'win', 'loss' or 'other', and an integer in {}:{}. Found '{}'.", this->fname,
                this->line_number, line);
            throw std::runtime_error("parse failure");
        }
        if (line_stream.fail())
        {
            CLOG("Expected an integer after 'win', 'loss' or 'other' in {}:{}. Found '{}'.", this->fname,
                this->line_number, line);
            throw std::runtime_error("parse failure");
        }
    }
    CLOG("Set points in case of a win to {}.", this->points_win);
    CLOG("Set points in case of a loss to {}.", this->points_loss);
    CLOG("Set points in other cases to {}.", this->points_other);
}

/******************************************************************************
 * Note the current standings in the tournament.
 *
 * @param fhandle Stream to read from.
 *****************************************************************************/
void
PointsTableProjector::parse_points_table(std::ifstream& fhandle)
{
    CLOG("Parsing points table.");
    std::string line;
    while (std::getline(fhandle, line))
    {
        ++this->line_number;
        if (line.empty())
        {
            break;
        }
        std::istringstream line_stream(line);
        std::string tname;
        int points;
        line_stream >> tname >> points;
        if (line_stream.fail())
        {
            CLOG("Expected a team and an integer in {}:{}. Found '{}'.", this->fname, this->line_number, line);
            throw std::runtime_error("parse failure");
        }
        std::size_t tid = this->tname_to_tid(tname);
        this->teams[tid].points = points;
        CLOG("Recorded '{}' with {} points.", tname, points);
    }
}

/******************************************************************************
 * Note the fixtures in the tournament.
 *
 * @param fhandle Stream to read from.
 * @param completed Whether the fixtures have been played or are to be played.
 *****************************************************************************/
void
PointsTableProjector::parse_fixture(std::ifstream& fhandle, bool completed)
{
    if (completed)
    {
        CLOG("Parsing completed fixtures.");
    }
    else
    {
        CLOG("Parsing upcoming fixtures.");
    }
    std::string line;
    while (std::getline(fhandle, line))
    {
        ++this->line_number;
        if (line.empty())
        {
            break;
        }
        std::size_t comma_idx = line.find(',');
        std::size_t equals_idx = line.find('=');
        if (comma_idx == equals_idx || (comma_idx != std::string::npos && equals_idx != std::string::npos))
        {
            CLOG("Expected two teams separated by either ',' or '=' in {}:{}. Found '{}'.", this->fname,
                this->line_number, line);
            throw std::runtime_error("parse failure");
        }
        std::size_t idx = comma_idx != std::string::npos ? comma_idx : equals_idx;
        std::size_t tid1 = this->tname_to_tid(line.substr(0, idx));
        std::size_t tid2 = this->tname_to_tid(line.substr(idx + 1, std::string::npos));
        if (completed)
        {
            if (line[idx] == '=')
            {
                this->teams[tid1].points += this->points_other;
                this->teams[tid2].points += this->points_other;
            }
            else
            {
                this->teams[tid1].points += this->points_win;
                this->teams[tid2].points += this->points_loss;
            }
        }
        else
        {
            this->upcoming_fixtures.emplace_back(Fixture(this->teams[tid1], this->teams[tid2]));
        }
        CLOG("Recorded fixture between '{}' and '{}'.", this->teams[tid1].tname, this->teams[tid2].tname);
    }
}

/******************************************************************************
 * Given a team name, obtain its ID. If there isn't one yet, create an ID for
 * it.
 *
 * @param tname Team name.
 *
 * @return Team ID.
 *****************************************************************************/
std::size_t
PointsTableProjector::tname_to_tid(std::string const& tname)
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
    std::cout << this->box_vertical_right << this->box_horizontal << this->section_begin << "table"
              << this->section_end << '\n';
    for (Team const& team : teams)
    {
        std::cout << this->box_vertical << " " << team << '\n';
    }
    std::cout << this->box_up_right << this->box_horizontal << this->section_begin << "upcoming" << this->section_end
              << '\n';
    for (Fixture const& fixture : this->upcoming_fixtures)
    {
        if (fixture.inconsequential)
        {
            std::cout << "  " << this->inconsequential_begin << fixture << this->inconsequential_end << '\n';
        }
        else
        {
            std::cout << "  " << fixture << '\n';
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
    for (Fixture const& fixture : this->upcoming_fixtures)
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
            min_max[fixture.a.tid][0] += this->points_loss;
            min_max[fixture.a.tid][1] += this->points_win;
        }
        if (fixture.b.tid != this->favourite_tid)
        {
            min_max[fixture.b.tid][0] += this->points_loss;
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
    for (Fixture& fixture : this->upcoming_fixtures)
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
    if (idx >= this->upcoming_fixtures.size())
    {
        this->dump();
        return;
    }

    static std::random_device rdev;
    static std::mt19937 mtwist(rdev());
    static auto bgen = [&]()
    {
        return static_cast<bool>(mtwist() & 1);
    };

    // If the outcome of this fixture does not matter, pick a winner randomly.
    Fixture& fixture = this->upcoming_fixtures[idx];
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
    loser.points += this->points_loss;
    this->solve_(idx + 1);
    loser.points -= this->points_loss;
    winner.points -= this->points_win;
}
