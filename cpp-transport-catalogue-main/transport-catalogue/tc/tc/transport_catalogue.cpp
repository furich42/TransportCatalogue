#include "transport_catalogue.h"



using namespace std;

namespace catalogue {


 
void TransportCatalogue::AddBus(Bus bus) {
    if(bus_for_adress.count(bus.number) != 0) {
        return;
    }
    buses.push_back(bus);
    bus_for_adress[bus.number] = &buses[buses.size() - 1];
    for(auto& key : bus.stops) {
        stop_for_buses[(*key).name].insert(bus.number);
    }
    

    
}

void TransportCatalogue::AddStop(Stop stop) {
    if(stop_for_adress.count(stop.name) != 0) {
        return;
    }
    stops.push_back(stop);
    stop_for_adress[stop.name] = &stops[stops.size() - 1];
    for(auto& [key, value] : stop.distances) {
        stops_distances[key] = value;
    }
}

const Bus* TransportCatalogue::FindBus(string& bus_number) {
    try {
        return bus_for_adress.at(bus_number);
    } catch(...) {
        return nullptr;
    }
}

 Stop* TransportCatalogue::FindStop(std::string& stop_name) {
    try {
        return stop_for_adress.at(stop_name);
    } catch(...) {
        return nullptr;
    }
}

BusStatistics TransportCatalogue::GetBusInfo(string& bus_number) {

    try {
        BusStatistics stat;
        stat.bus = bus_for_adress.at(bus_number)->number;
        stat.stops_amount = static_cast<int>(bus_for_adress.at(bus_number)->stops.size());
        vector<Stop*> unique_stops_local = bus_for_adress.at(bus_number)->stops;
        
        sort(unique_stops_local.begin(), unique_stops_local.end(), [](auto a, auto b) {
            return a->name < b->name;
        });
        
        auto last = unique(unique_stops_local.begin(), unique_stops_local.end(), [](auto a, auto b) {
            return a->name == b->name;
        });
        unique_stops_local.erase(last, unique_stops_local.end());
        stat.unique_stops = static_cast<int>(unique_stops_local.size());
        stat.distance = bus_for_adress.at(bus_number)->dist;
        stat.length = bus_for_adress.at(bus_number)->length;
        stat.check = true;
        return stat;
    } catch (...) {
        BusStatistics stat;
        stat.bus = bus_number;
        return stat;
    }
}
    
double TransportCatalogue::CalculateDistance(vector<Stop*>& bus_stops) {
    double dist = 0;
        for (size_t i = 1; i < bus_stops.size(); i++) {
            TwoStops two_stops;
            two_stops.stop1 = (bus_stops[i-1])->name;
            two_stops.stop2 = (bus_stops[i])->name;
            try{
                dist += stops_distances.at(two_stops);
            } catch(...) {
                swap(two_stops.stop1, two_stops.stop2);
                dist += stops_distances.at(two_stops);
            }
        }

    return dist;
}
    
double TransportCatalogue::CalculateLength(vector<Stop*>& bus_stops) {
    double length = 0;
    for (size_t i = 1; i < bus_stops.size(); i++) {
        length += ComputeDistance(bus_stops[i-1]->coordinates, bus_stops[i]->coordinates);
    }

    return length;
}

StopStatistics TransportCatalogue::GetStopInfo(string& stop_name) {

    try {
        StopStatistics stat;
        stat.name = stop_for_adress.at(stop_name)->name;
        try {
            stat.buses = stop_for_buses.at(stop_name);
        } catch (...) {
            stat.buses = {};
        }
        stat.check = true;
        return stat;
    } catch (...) {
        StopStatistics stat;
        stat.name = stop_name;
        return stat;
    }
}
    
    std::vector<domain::Bus*> TransportCatalogue::GetAllBuses() {
            std::vector<domain::Bus*> buses;
            for(auto [key, value] : bus_for_adress) {
                buses.push_back(value);
            }
            std::sort(buses.begin(), buses.end(), [](domain::Bus* a, domain::Bus* b) {
    return std::lexicographical_compare(a->number.begin(), a->number.end(), b->number.begin(), b->number.end());
            });
            return buses;
        }
    
    std::vector<domain::Stop*> TransportCatalogue::GetAllStopsWithBus() {
            std::vector<domain::Stop*> stops;
            for(auto [key, value] : stop_for_adress) {
                if(GetStopInfo(value->name).buses.size() > 0) {
                    stops.push_back(value);
                }
                
            }
            std::sort(stops.begin(), stops.end(), [](domain::Stop* a, domain::Stop* b) {
    return std::lexicographical_compare(a->name.begin(), a->name.end(), b->name.begin(), b->name.end());    
});
            return stops;
        }


}
