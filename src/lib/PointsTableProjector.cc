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

#define THROW(exc, msg)  \
do  \
{  \
    throw exc(std::string(__FILE__) + ':' + std::to_string(__LINE__) + " in " + __func__ + ". " + msg);  \
}  \
while(false)

/******************************************************************************
 * Constructor.
 *
 * @param fname Input file name.
 *****************************************************************************/
PointsTableProjector::PointsTableProjector(std::string const& fname)
: fname(fname)
{
    this->parse();
    this->debug();
}

/******************************************************************************
 * Read the contents of the input file and initialise all members accordingly.
 *****************************************************************************/
void
PointsTableProjector::parse(void)
{
    std::ifstream fhandle(this->fname);
    if(!fhandle.good() || !std::filesystem::is_regular_file(this->fname))
    {
        THROW(std::runtime_error, "Could not read '" + this->fname + "'.");
    }
    std::string my_tname;
    while(true)
    {
        std::string readline;
        ++this->line_number;
        if(!std::getline(fhandle, readline))
        {
            break;
        }
        if(readline.empty())
        {
            continue;
        }
        if(readline.rfind("points.win", 0) == 0)
        {
            this->parse_int(&readline[10], this->points_win);
            continue;
        }
        if(readline.rfind("points.lose", 0) == 0)
        {
            this->parse_int(&readline[11], this->points_lose);
            continue;
        }
        if(readline.rfind("points.other", 0) == 0)
        {
            this->parse_int(&readline[12], this->points_other);
            continue;
        }
        if(readline.rfind("team ", 0) == 0)
        {
            my_tname = readline.substr(5, std::string::npos);
            continue;
        }
        if(readline == "fixtures.completed")
        {
            while(std::getline(fhandle, readline) && !readline.empty())
            {
                ++this->line_number;
                this->parse_fixture(readline, true);
            }
            ++this->line_number;
            continue;
        }
        if(readline == "fixtures.upcoming")
        {
            while(std::getline(fhandle, readline) && !readline.empty())
            {
                ++this->line_number;
                this->parse_fixture(readline, false);
            }
            ++this->line_number;
            continue;
        }
        std::string message = "Unknown keyword in '" + this->fname;
        message += "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
    }
    if(my_tname.empty())
    {
        std::string message = "Team to track not specified in '" + this->fname + "'.";
        THROW(std::runtime_error, message);
    }
    auto tname_tid_it = this->tname_tid.find(my_tname);
    if(tname_tid_it == this->tname_tid.end())
    {
        std::string message = "No fixtures involving '" + my_tname + "' found in '" + this->fname + "'.";
        THROW(std::runtime_error, message);
    }
    this->my_tid = tname_tid_it->second;
}

/******************************************************************************
 * Store an integer in the specified variable.
 *
 * @param str String to parse.
 * @param intvar Variable to store the result in.
 *****************************************************************************/
void
PointsTableProjector::parse_int(char const *str, int& intvar)
{
    try
    {
        intvar = std::stoi(str);
    }
    catch(std::exception& e)
    {
        std::string message = "Integer parse failure in '" + this->fname;
        message += "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
    }
}

/******************************************************************************
 * Store information about a match.
 *
 * @param str String to parse.
 * @param update_points Whether to update the points table.
 *****************************************************************************/
void
PointsTableProjector::parse_fixture(std::string const& str, bool update_points)
{
    auto delim_idx = str.find_first_of(",=");
    if(delim_idx == std::string::npos)
    {
        std::string message = "Neither ',' nor '=' found in '" + this->fname;
        message += "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
    }
    std::size_t first_tid = this->reg(str.substr(0, delim_idx));
    std::size_t second_tid = this->reg(str.substr(delim_idx + 1, std::string::npos));
    if(update_points)
    {
        if(str[delim_idx] == '=')
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
        this->fixtures_upcoming.emplace_back(Fixture(this->teams[first_tid], this->teams[second_tid]));
    }
}

/******************************************************************************
 * Register a team by generating an ID for it if it isn't already registered.
 *
 * @param tname Team name.
 *
 * @return Team ID.
 *****************************************************************************/
std::size_t
PointsTableProjector::reg(std::string const& tname)
{
    auto tname_tid_it = this->tname_tid.find(tname);
    if(tname_tid_it == this->tname_tid.end())
    {
        std::size_t tid = this->tname_tid.size();
        this->tname_tid.insert({tname, tid});
        this->teams.emplace_back(Team(tname, tid));
        return tid;
    }
    return tname_tid_it->second;
}

/******************************************************************************
 * Display all members.
 *****************************************************************************/
void
PointsTableProjector::debug(void)
{
    std::clog << "File: '" << this->fname << "'\n";
    std::clog << "Points: (" << this->points_win << ", " << this->points_lose << ", " << this->points_other << ")\n";

    std::clog << "Teams:";
    for(std::size_t i = 0; i < this->teams.size(); ++i)
    {
        std::clog << ' ';
        if(i == this->my_tid)
        {
            std::clog << '*';
        }
        std::clog << teams[i].name << ':' << teams[i].points;
    }
    std::clog << '\n';

    std::clog << "Upcoming:";
    for(auto const& fixture: this->fixtures_upcoming)
    {
        std::clog << ' ' << fixture.a.name << ',' << fixture.b.name;
    }
    std::clog << '\n';
}
