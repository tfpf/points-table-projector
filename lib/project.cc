#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
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
    void parse_fixture(std::string& str, std::vector<std::vector<int>>& vecvar);

    private:
    char const *fname;
    int line_number = 0;
    int points_win = 2;
    int points_lose = 0;
    int points_other = 1;
    std::vector<std::vector<int>> fixtures_completed;
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
        THROW(std::runtime_error, "Could not read '" + fname + "'.");
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
            // Only update the points table. Don't store this. Store only upcoming fixtures.
            while(std::getline(fhandle, readline) && !readline.empty())
            {
                ++this->line_number;
                this->parse_fixture(readline, this->fixtures_completed);
            }
        }
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
        message += std::string(fname) + "' on line " + std::to_string(line_number) + '.';
        THROW(std::invalid_argument, message);
    }
}

/******************************************************************************
 * Store information about a match in the specified variable.
 *
 * @param str String to parse.
 * @param vecvar Variable to store the result in.
 *****************************************************************************/
void
PointsTableProjector::parse_fixture(std::string& str, std::vector<std::vector<int>>& vecvar)
{
    auto comma = str.find(',');
    auto equals = str.find('=');
    if(comma != std::string::npos && equals != std::string::npos)
    {
        std::string message("Ambiguous outcomes (comma and equals) found in '");
        message += std::string(fname) + "' on line " + std::to_string(line_number) + '.';
        THROW(std::invalid_argument, message);
    }
    auto delimiter = comma != std::string::npos ? comma : equals;
    std::string first_team = str.substr(0, delimiter);
    std::string second_team = str.substr(delimiter + 1, std::string::npos);
    std::cout << first_team << ' ' << second_team << '\n';
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
