

#include <algorithm>
#include <cassert>
// #include <cstdint>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <getopt.h>

using namespace std;



class Hunt{
    public:

    struct Location{
        uint32_t row;
        uint32_t col;
        char terrain;
    };
    
    // reads options in from command line
    void get_options(int argc, char** argv);


    void read_data();


    void sail();

    void search();


    Location hunt(char const &c, Location current) {
        Location temp = current;
        if (c == 'N') {
            if (current.row > 0) {
                temp.row = (current.row - 1);
            }
            else temp.terrain = '0';
        }
        else if (c == 'E') {
            if (current.col < (map_size - 1)) {
                temp.col = (current.col + 1);
            }
            else temp.terrain = '0';
        }
        else if (c == 'S') {
            if (current.row < (map_size - 1)) {
                temp.row = (current.row + 1);
            }
            else temp.terrain = '0';
        }
        else if (c == 'W') {
            if (current.col > 0) {
                temp.col = (current.col - 1);
            }
            else temp.terrain = '0';
        }
        temp.terrain = map[temp.row][temp.col];
        return temp;
    }


    void back_trace();


    void print_stats();


    void print_results();

    void print_path();

    bool is_on_path(const char c);
    
    
    private:
    Location start;
    Location current_sail;
    Location current_search;
    Location treasure;
    deque <Location> captain_container;
    deque <Location> firstmate_container;
    stack <Location> path_list;
    vector<vector<char>> map;
    vector<vector<char>> path_map;
    uint32_t map_size;
    uint32_t num_land_inv = 0;
    uint32_t num_water_inv = 0;
    uint32_t path_length = 0;
    uint32_t num_ashores = 0;
    uint8_t Ndex = 0;
    uint8_t Sdex = 0;
    uint8_t Edex = 0;
    uint8_t Wdex = 0;
    string sail_container_type = "STACK";
    string search_container_type = "QUEUE";
    string hunt_order = "NESW";
    string show_path = "";
    bool verbose = false;
    bool stats = false;
    char input_type;
};


ostream &operator<<(std::ostream &os, Hunt::Location L);