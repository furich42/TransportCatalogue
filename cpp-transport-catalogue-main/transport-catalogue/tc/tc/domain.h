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
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

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
