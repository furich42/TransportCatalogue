#pragma once
#include "geo.h"
#include <vector>
#include <string>

using geo::Coordinates;
using geo::ComputeDistance;

struct Stop {
    std::string name_;
    Coordinates cordinates_;
    int id = 0;
};

enum class BusType {
    ORDINARY,
    CYCLED,
    MAX_BUS_TYPES,
};

struct Bus {
    std::string name_;
    std::vector<const Stop*> stops_;
    BusType bus_type_;
    int id = 0;
};

struct CoordinatesHasher {
    size_t operator()(geo::Coordinates c) const {
        std::hash<double> ptr_hasher{}; // объект - хешер
        static const size_t PRIME = 17;
        return PRIME * ptr_hasher(c.lat) + ptr_hasher(c.lng);
    }
};

struct BusRouteWeight {
    double time = 0.0;
    int span = 0;

    BusRouteWeight() = default;

    bool operator<(const BusRouteWeight& other) const {
        return time < other.time;
    }

    bool operator>(const BusRouteWeight& other) const {
        return time > other.time;
    }

    bool operator<=(const BusRouteWeight& other) const {
        return !(time > other.time);
    }

    bool operator>=(const BusRouteWeight& other) const {
        return !(time < other.time);
    }

    bool operator==(const BusRouteWeight& other) const {
        return time == other.time;
    }

    bool operator!=(const BusRouteWeight& other) const {
        return time != other.time;
    }

    BusRouteWeight operator+(const BusRouteWeight& other) const {
        return {time + other.time, span + other.span};
    }

};

constexpr double BUS_VELOCITY_MULTIPLIER = 100.0 / 6.0;
