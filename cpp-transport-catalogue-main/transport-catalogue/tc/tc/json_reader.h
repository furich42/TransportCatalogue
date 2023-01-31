#pragma once
#include "transport_catalogue.h"
#include <string>
#include <sstream>
#include <iostream>
#include <istream>
#include <vector>
#include <algorithm>
#include <thread>

#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"


namespace input {
    
using namespace catalogue;

struct InputCall {
    std::string call = "";
    std::string info = "";
};


//-------------------------------IN-JSON_READER-----------------
    
class JSONReader {
public:
    Bus ReadBusFromJSON( json::Dict& info, catalogue::TransportCatalogue& catalogue);

Stop ReadStopFromJSON(json::Dict& info);
    
renderer::Settings ReadSettingsFromJSON(json::Dict& info);

void ReadStatRequests(json::Dict& dc, std::ostream& out, catalogue::TransportCatalogue& catalogue);
void ReadBaseRequests(json::Dict& dc, catalogue::TransportCatalogue& catalogue);    
void ReadRequests(std::istream& input, std::ostream& out, catalogue::TransportCatalogue& catalogue);
void ReadSettingsRequests(json::Dict& dc, renderer::Settings& settings);
void ReadStatRequests(json::Dict& dc, std::ostream& out, catalogue::TransportCatalogue& catalogue, renderer::Settings& settings);
};




//-------------------------------END-IN-JSON_READER-----------------


InputCall ReadInput( std::string_view input);
    
Stop ReadStop(std::string_view input);
    
Bus ReadBus(std::string_view input, catalogue::TransportCatalogue& catalogue);
    
void InsertBase(catalogue::TransportCatalogue& catalogue, std::istream& input_stream);
    
}

