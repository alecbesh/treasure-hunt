

#include <algorithm>
#include <cassert>
#include <cstdint>
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






int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);

    Hunt h;

    h.get_options(argc, argv);
    h.read_data();
    h.sail();
    h.print_results();

    return 0;
}