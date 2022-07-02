#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */



using namespace std;

namespace stat {

using namespace catalogue;

//---------------------------------JSON_PRINTER-----------------
    
    


void JSONPrinter::PrintJSONStop(std::ostream& out, std::string name, int id, catalogue::TransportCatalogue& catalogue) {
    
    
    
    out << "{\"";
    StopStatistics stat = catalogue.GetStopInfo(name);
    if(stat.check) {
        out << "buses\": [";
        
        bool first = true;
        for(const std::string& stop : stat.buses) {
            if(first) {
                first = false;
            } else {
                out << ", ";
            }
            out << "\"" << stop << "\"";
        }
        out << "], ";
    } else {
        out << "error_message\": " << "\"not found\", ";
    }
    
    
    out << "\"request_id\": ";
    out << id;
    out << "}";
}


void JSONPrinter::PrintJSONBus(std::ostream& out, std::string name, int id, catalogue::TransportCatalogue& catalogue) {
    
    BusStatistics stat = catalogue.GetBusInfo(name);
    if(stat.check) {
        json::Print(
            json::Document{
                json::Builder{}
                .StartDict()
                    .Key("curvature").Value(stat.distance/stat.length)
                    .Key("request_id").Value(id)
                    .Key("route_length").Value(stat.distance)
                    .Key("stop_count").Value(stat.stops_amount)
                    .Key("unique_stop_count").Value(stat.unique_stops)
                .EndDict()
                .Build()
            },
            out
        );
        
    } else {
        
        json::Print(
            json::Document{
                json::Builder{}
                .StartDict()
                    .Key("request_id").Value(id)
                    .Key("error_message").Value("not found")
                .EndDict()
                .Build()
            },
            out
        );
    }
    
}

void JSONPrinter::PrintMapJSON(std::ostream& out, int id, catalogue::TransportCatalogue& catalogue, renderer::Settings& settings) {
    renderer::MapRenderer maprender(settings, catalogue.GetAllBuses(), catalogue.GetAllStopsWithBus());
    std::ostringstream outstr;
    
    json::Print(
        json::Document{
            json::Builder{}
            .StartDict()
                .Key("map").Value(maprender.FormMap(outstr).str())
                .Key("request_id").Value(id)
            .EndDict()
            .Build()
        },
        out
    );
    
    
    
}



//-------------------------------END-JSON_PRINTER-----------------


using namespace catalogue;
    

    
void JSONPrinter::PrintBusInfo(BusStatistics info, ostream& output_stream) {
    if(!info.check) {
//        json::Print(
//            json::Document{
//                json::Builder{}
//                .Build()
//            },
//            out
//        );
        
        output_stream << "Bus " << info.bus <<": " << "not found\n";
        return;
    }
    output_stream << "Bus " << info.bus << ": " << info.stops_amount << " stops on route, " << info.unique_stops << " unique stops, " << info.distance << " route length, " << std::setprecision(6) << info.distance / info.length << " curvature\n";
}

void JSONPrinter::PrintStopInfo(StopStatistics info, ostream& output_stream) {
    if(!info.check) {
        output_stream << "Stop " << info.name <<": " << "not found\n";
        return;
    }
    if(info.buses.size() > 0) {
        output_stream << "Stop " << info.name << ": " << "buses " ;
        
        for(auto& name : info.buses) {
            output_stream << name << " ";
        }
        output_stream << "\n";
    } else {
        output_stream << "Stop " << info.name << ": no buses\n";
    }
}

void JSONPrinter::ReadAndProcessQueries( TransportCatalogue& catalogue, istream& input_stream, ostream& output_stream) {
    int mn;
    input_stream >> mn;
    string input_st;
    getline(input_stream, input_st);
    for(int i = 0; i < mn; ++i) {
        getline(input_stream, input_st);
        string_view input = input_st;
        string call;
        call = {&input[0], &input[input.find(' ')]};
        if(call == "Bus") {
            while(*input.begin() == ' ') {
                input = input.substr(1);
            }
            string number = {&input[input.find(' ') + 1], &input[input.size()]};
            PrintBusInfo(catalogue.GetBusInfo(number), output_stream);
        } else if (call == "Stop") {
            while(*input.begin() == ' ') {
                input = input.substr(1);
            }
            string st_name = {&input[input.find(' ') + 1], &input[input.size()]};
            PrintStopInfo(catalogue.GetStopInfo(st_name), output_stream);
        }
    }
}
}


