#include <string>
#include <unordered_map>
#include <vector>

class PointsTableProjector
{
    public:
    PointsTableProjector(std::string const& fname);
    void parse(void);
    void parse_int(char const *str, int& var);
    void parse_fixture(std::string const& str, bool update_points);
    int reg(std::string const& tname);
    void debug(void);

    private:
    std::string const fname;
    int line_number = 0;
    int points_win = 2;
    int points_lose = 0;
    int points_other = 1;
    int my_tid = 0;
    std::unordered_map<std::string, int const> tname_tid;
    std::vector<int> tid_points;
    std::vector<std::vector<int>> fixtures_upcoming;
};
