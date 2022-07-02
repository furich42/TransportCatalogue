#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"
#include <string>
#include <sstream>
#include <iostream>
#include <ostream>
#include <iomanip>
#include "json_builder.h"


namespace stat {
    
class JSONPrinter {
public:
    void PrintBusInfo(catalogue::BusStatistics info, std::ostream& output_stream);
    void PrintStopInfo(catalogue::StopStatistics info, std::ostream& output_stream);
    void ReadAndProcessQueries(catalogue::TransportCatalogue& catalogue, std::istream& input_stream, std::ostream& output_stream);

    //---------------------------------JSON_PRINTER-----------------


    void PrintJSONStop(std::ostream& out, std::string name, int id, catalogue::TransportCatalogue& catalogue);


    void PrintJSONBus(std::ostream& out, std::string name, int id, catalogue::TransportCatalogue& catalogue);

    void PrintMapJSON(std::ostream& out, int id, catalogue::TransportCatalogue& catalogue, renderer::Settings& settings );

//-------------------------------END-JSON_PRINTER-----------------
};


    
}

