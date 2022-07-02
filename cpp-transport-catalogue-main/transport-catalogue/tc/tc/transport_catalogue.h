#pragma once

/*
 * Здесь можно разместить код транспортного справочника
 */
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
#include "domain.h"


namespace catalogue {
    
using namespace domain;



class TransportCatalogue {
   
    public:
    
        void AddBus(Bus bus);

        void AddStop(Stop stop);

        const Bus* FindBus(std::string& bus_number);

        Stop* FindStop(std::string& stop_name);

        BusStatistics GetBusInfo(std::string& bus_number);
    
        StopStatistics GetStopInfo(std::string& stop_name);
    
        double CalculateDistance(std::vector<Stop*>& bus_name);
    
        double CalculateLength(std::vector<Stop*>& bus_name);
    
        std::vector<domain::Bus*> GetAllBuses();
    
        std::vector<domain::Stop*> GetAllStopsWithBus() ;
    
    private:
        std::unordered_map<std::string, Bus*> bus_for_adress;
        std::unordered_map<std::string, Stop*> stop_for_adress;
        std::unordered_map<std::string, std::set<std::string>> stop_for_buses;
        std::unordered_map<TwoStops, double, StopsHasher> stops_distances;
        std::deque<Bus> buses;
        std::deque<Stop> stops;
};

}



