

#include <algorithm>
#include <cassert>
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
#include "hunt.h"

using namespace std;

void Hunt::get_options(int argc, char**argv) {
    int option_index = 0, option = 0;
    opterr = false;


    // use getopt to find command line options
    struct option longOpts[] = {{ "captain", 1, nullptr, 'c' },
                                { "first-mate", 1, nullptr, 'f' },
                                { "hunt-order", 1, nullptr, 'o' },
                                { "verbose", 0, nullptr, 'v' },
                                { "stats", 0, nullptr, 's' },
                                { "show-path", 1, nullptr, 'p' },
                                { "help", 0, nullptr, 'h' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "c:f:o:vsp:h", longOpts, &option_index)) != -1) {
        switch (option) {

            case 'c':
                sail_container_type = optarg;
                if (sail_container_type != "QUEUE" && sail_container_type != "STACK") exit(1);
                break;

            case 'f':
                search_container_type = optarg;
                if (search_container_type != "QUEUE" && search_container_type != "STACK") exit(1);
                break;

            case 'o':
                hunt_order = optarg;
                if (hunt_order.length() != 4) exit(1);
                for (const char &c : hunt_order) {
                    if (c != 'N' && c != 'E' && c != 'S' && c != 'W') {
                        exit(1);
                    }
                    if (c == 'N') Ndex++;
                    else if (c == 'S') Sdex++;
                    else if (c == 'E') Edex++;
                    else if (c == 'W') Wdex++;
                }
                if (Ndex != 1 || Sdex != 1 || Edex != 1 || Wdex != 1) exit(1);
                break;

            case 'v':
                verbose = true;
                break;

            case 's':
                stats = true;
                break;

            case 'p':
                if (show_path == "M" || show_path == "L") exit(1);
                show_path = optarg;
                if (show_path.length() != 1) exit(1);
                if (show_path != "M" && show_path != "L") exit(1);
                break;
            
            case 'h':
                cout << "This program reads input files of a map that contains different\n"
                     << "landmarks: water(.), land(o), impassable(#), starting-location(@),\n"
                     <<  "and treasure(#).  If treasure is found, it then outputs\n"
                     <<  "where exactly this treasure was found.\n"
                          <<  "Usage: \'./hunt\n\t[--help | -h]\n"
                          <<                   "\t[--captain | -c]\n"
                          <<                   "\t[--first-mate | -f]\n"
                          <<                   "\t[--hunt-order | -h]\n"
                          <<                   "\t[--verbose | -v]\n"
                          <<                   "\t[--stats | -s]\n"
                          <<                   "\t[--show-path | -p]\n";
                exit(0);

            // All short/long options not in the spec should result in program term. w non zero exit code
            default:
                exit(1);
        }
    }
}



void Hunt::read_data() {
    treasure.terrain = '\0';
    string trash;
    cin >> input_type;
    while (input_type == '#') {
        getline(cin, trash);
        cin >> input_type;
    }
    cin >> map_size;
    vector<char> dots(map_size, '.');
    for (uint32_t i = 0; i < map_size; ++i) {
        map.push_back(dots);
    }
    if (input_type == 'M') {
        for (uint32_t i = 0; i < map_size; ++i) {
            for (uint32_t j = 0; j < map_size; ++j) {
                cin >> map[i][j];
                if (map[i][j] == '@') {
                    current_sail.row = i;
                    current_sail.col = j;
                    current_sail.terrain = '@';
                    captain_container.push_front(current_sail);
                }
            }
        }
    }
    else if (input_type == 'L') {
        uint32_t row;
        uint32_t col;
        char terrain;
        while (cin >> row && cin >> col && cin >> terrain) {
            map[row][col] = terrain;
            if (terrain == '@') {
                current_sail.row = row;
                current_sail.col = col;
                current_sail.terrain = terrain;
                captain_container.push_front(current_sail);
            }
        }
    }
    path_map.resize(map_size, vector<char>(map_size, '0'));
    path_map = map;
}





void Hunt::sail() {
    Location temp;
    start = current_sail;
    if (verbose) cout << "Treasure hunt started at: " << start << "\n";
    while (!captain_container.empty() && treasure.terrain != '$') {
        current_sail = captain_container.front();
        captain_container.pop_front();
        num_water_inv++;
        for (char const &c : hunt_order) {
            temp = hunt(c, current_sail);
            if (temp.terrain == 'o' || temp.terrain == '$') {
                if (search_container_type == "QUEUE") firstmate_container.push_back(temp);
                else if (search_container_type == "STACK") firstmate_container.push_front(temp);
                map[temp.row][temp.col] = c;
                search();
                num_ashores++;
                if (current_search.terrain == '$') return;
            }
            else if (temp.terrain == '.') {
                if (sail_container_type == "QUEUE") captain_container.push_back(temp);
                else if (sail_container_type == "STACK") captain_container.push_front(temp);
                map[temp.row][temp.col] = c;
            }
        }
    }
}

void Hunt::search() {
    Location temp;
    if (verbose) cout << "Went ashore at: " << firstmate_container.front() << "\n";
    while (!firstmate_container.empty()) {
        current_search = firstmate_container.front();
        firstmate_container.pop_front();
        num_land_inv++;
        if (current_search.terrain == '$') {
                treasure.row = current_search.row;
                treasure.col = current_search.col;
                treasure.terrain = current_search.terrain;
                if (verbose) cout << "Searching island... party found treasure at " << treasure << ".\n";
                return;
        }

        for (char const &c : hunt_order) {
            temp = hunt(c, current_search);
            if (temp.terrain == 'o' || temp.terrain == '$') {
                if (search_container_type == "QUEUE") firstmate_container.push_back(temp);
                else if (search_container_type == "STACK") firstmate_container.push_front(temp);
                map[temp.row][temp.col] = c;
            }
            if (temp.terrain == '$') {
                current_search = temp;
                treasure.row = current_search.row;
                treasure.col = current_search.col;
                treasure.terrain = current_search.terrain;
                if (verbose) cout << "Searching island... party found treasure at " << treasure << ".\n";
                num_land_inv++;
                return;
            }
        }
    }
    if (verbose) {
        cout << "Searching island... party returned with no treasure.\n";
    }
}



void Hunt::back_trace() {
    Location temp = treasure;
    Location temp2;
    path_list.push(temp);
    while (temp.terrain != '@') {
        if (map[temp.row][temp.col] == 'N') {
            path_map[temp.row][temp.col] = 'N';
            temp.row += 1;
            temp.terrain = map[temp.row][temp.col];
            path_length++;
        }
        else if (map[temp.row][temp.col] == 'E') {
            path_map[temp.row][temp.col] = 'E';
            temp.col -= 1;
            temp.terrain = map[temp.row][temp.col];
            path_length++;
        }
        else if (map[temp.row][temp.col] == 'S') {
            path_map[temp.row][temp.col] = 'S';
            temp.row -= 1;
            temp.terrain = map[temp.row][temp.col];
            path_length++;
        }
        else if (map[temp.row][temp.col] == 'W') {
            path_map[temp.row][temp.col] = 'W';
            temp.col += 1;
            temp.terrain = map[temp.row][temp.col];
            path_length++;
        }
        temp2.row = temp.row;
        temp2.col = temp.col;
        temp2.terrain = path_map[temp.row][temp.col];
        path_list.push(temp2);
    }
    path_map[treasure.row][treasure.col] = '$';
}


void Hunt::print_stats(){
    cout << "--- STATS ---\n";
    cout << "Starting location: " << start << "\n";
    cout << "Water locations investigated: " << num_water_inv << "\n";
    cout << "Land locations investigated: " << num_land_inv << "\n";
    cout << "Went ashore: " << num_ashores << "\n";
    if (treasure.terrain == '$') {
        cout << "Path length: " << path_length << "\n";
        cout << "Treasure location: " << treasure << "\n";
    }
    cout << "--- STATS ---\n";
}


void Hunt::print_results() {
    int num_locations = num_land_inv + num_water_inv;
    if (verbose && treasure.terrain != '$') cout << "Treasure hunt failed\n"; 
    if (treasure.terrain == '$') {
        back_trace();
        if (stats) print_stats();
        if (show_path != "") print_path();
        cout << "Treasure found at " << treasure << " with path length " << path_length << ".\n";
    }
    else {
        if (stats) print_stats();
        cout << "No treasure found after investigating " << num_locations << " locations.\n";
    }
}



bool Hunt::is_on_path(const char c) {
    if (c == 'N' || c == 'S' || c == 'E' || c == 'W' || c == 'X' 
        || c == '@' || c == '$' || c == '|' || c == '-' || c == '+') {
        return true;
    }
    else {
        return false;
    }
}





void Hunt::print_path() {
    if (treasure.terrain != '$') show_path = "";
    if (show_path == "L") {
        cout << "Sail:\n";
        cout << path_list.top() << "\n";
        path_list.pop();
        while (path_list.top().terrain == '.') {
            cout << path_list.top() << "\n";
            path_list.pop();
        }

        cout << "Search:\n";
        while (!path_list.empty()) {
            cout << path_list.top() << "\n";
            path_list.pop();
        }
    }

    if (show_path == "M") {
        for (uint32_t row = 0; row < map_size; ++row) {
            for (uint32_t col = 0; col < map_size; ++col) {
                if (path_map[row][col] == '$') path_map[row][col] = 'X';

                else if (path_map[row][col] == 'N' || path_map[row][col] == 'S') {
                    if (row > 0 && row < (map_size - 1)) {
                        if (is_on_path(path_map[row-1][col]) && is_on_path(path_map[row+1][col])) {
                            path_map[row][col] = '|';
                        }
                        else {
                            path_map[row][col] = '+';
                        }
                    }
                    else {
                        path_map[row][col] = '+';
                    }
                }

                else if (path_map[row][col] == 'W' || path_map[row][col] == 'E') {
                    if (col > 0 && col < (map_size - 1)) {
                        if (is_on_path(path_map[row][col-1]) && is_on_path(path_map[row][col+1])) {
                            path_map[row][col] = '-';
                        }
                        else {
                            path_map[row][col] = '+';
                        }
                    }
                    else {
                        path_map[row][col] = '+';
                    }
                }
                cout << path_map[row][col]; 
            }
            cout << "\n";
        }
    }
}




ostream &operator<<(std::ostream &os, Hunt::Location L) {
    os << L.row << "," << L.col;
    return os;
}