#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#define THROW(exc, msg)  \
do  \
{  \
    throw exc(std::string(__FILE__) + ':' + std::to_string(__LINE__) + " in " + __func__ + ". " + msg);  \
}  \
while(false)

/******************************************************************************
 * Determine the best outcome for a team in an ongoing tournament.
 *****************************************************************************/
class PointsTableProjector
{
    public:
    PointsTableProjector(char const *fname);
    void parse(void);
    void parse_int(char const *str, int& var);
    void parse_fixture(std::string const& str, bool update_points);
    int reg(std::string const& tname);

    private:
    char const *fname;
    int line_number = 0;
    int points_win = 2;
    int points_lose = 0;
    int points_other = 1;
    std::unordered_map<std::string, int const> team_id;
    std::vector<int> team_points;
    std::vector<std::vector<int>> fixtures_upcoming;
};

/******************************************************************************
 * Constructor.
 *
 * @param fname Input file name.
 *****************************************************************************/
PointsTableProjector::PointsTableProjector(char const *fname)
{
    this->fname = fname;
    this->parse();
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
        std::string message("Unknown keyword in '");
        message += std::string(this->fname) + "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
    }
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
        std::string message("Integer parse failure in '");
        message += std::string(this->fname) + "' on line " + std::to_string(this->line_number) + '.';
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
        std::string message("Neither ',' nor '=' found in '");
        message += std::string(this->fname) + "' on line " + std::to_string(this->line_number) + '.';
        THROW(std::invalid_argument, message);
    }
    int first_team = this->reg(str.substr(0, delim_idx));
    int second_team = this->reg(str.substr(delim_idx + 1, std::string::npos));
    if(update_points)
    {
        if(str[delim_idx] == '=')
        {
            this->team_points[first_team] += this->points_other;
            this->team_points[second_team] += this->points_other;
        }
        else
        {
            this->team_points[first_team] += this->points_win;
            this->team_points[second_team] += this->points_lose;
        }
    }
    else
    {
        this->fixtures_upcoming.push_back({first_team, second_team, -1});
    }
}

/******************************************************************************
 * Register a team by generating an ID for it if it isn't already registered.
 *
 * @param tname Team name.
 *
 * @return Team ID.
 *****************************************************************************/
int
PointsTableProjector::reg(std::string const& tname)
{
    if(this->team_id.find(tname) == this->team_id.end())
    {
        this->team_id.insert({tname, this->team_id.size()});
        this->team_points.push_back(0);
    }
    return this->team_id[tname];
}

/******************************************************************************
 * Main function.
 *****************************************************************************/
int
main(int const argc, char const *argv[])
{
    if(argc < 2)
    {
        THROW(std::invalid_argument, "Input file not specified.");
    }
    PointsTableProjector projector(argv[1]);
}
