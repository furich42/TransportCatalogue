#include "domain.h"

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
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
