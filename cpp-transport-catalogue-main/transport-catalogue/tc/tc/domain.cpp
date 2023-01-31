#include "domain.h"

using namespace std;

namespace domain {

bool operator==(BusStatistics lhs, BusStatistics rhs) {
        return lhs.bus == rhs.bus && lhs.stops_amount == rhs.stops_amount && lhs.unique_stops == rhs.unique_stops && lhs.length == rhs.length;
    }

bool operator==(TwoStops lhs, TwoStops rhs) {
    return lhs.stop1 == rhs.stop1 && lhs.stop2 == rhs.stop2;
}

string TwoStops::Hash() const {
    string twostops = stop1;
    for(char c : stop2) {
        twostops.push_back(c);
    }
    return twostops;
}

size_t StopsHasher::operator()(const TwoStops& stops) const {
    return hasher_(stops.Hash());
}


}
