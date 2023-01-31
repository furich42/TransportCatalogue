#pragma once
#include <vector>
#include <string>
#include <istream>
#include <sstream>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <set>
#include <string_view>
#include <functional>
#include <cctype>
#include <locale>

#include "geo.h"

namespace domain {

class TwoStops {
    public:
        std::string stop1 = "";
        std::string stop2 = "";
        
        std::string Hash() const;
};

class StopsHasher {
    public:
        size_t operator()(const TwoStops& stops) const;
    
    private:
        std::hash<std::string> hasher_;
};



struct Stop {
    std::string name;
    geo::Coordinates coordinates;
    std::unordered_map<TwoStops, double, StopsHasher> distances;
};


struct Bus {
    std::string number;
    double dist = 0;
    double length = 0;
    std::vector<Stop*> stops;
    bool round;
};

struct BusStatistics {
    std::string bus = "";
    int stops_amount = 0;
    int unique_stops = 0;
    double length = 0;
    double distance = 0;
    bool check = false;
};

struct StopStatistics {
    std::string name = "";
    std::set<std::string> buses;
    bool check = false;
};
    
bool operator==(StopStatistics lhs, StopStatistics rhs);
bool operator==(TwoStops lhs, TwoStops rhs);
    



}
