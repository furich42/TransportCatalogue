#include "json_reader.h"
#include <iomanip>

using namespace std;

using namespace json;
using namespace std::literals;

std::string Print(const Node& node) {
    std::ostringstream out;
    Print(json::Document{node}, out);
    return out.str();
}


namespace input {


//-------------------------------IN-JSON_READER-----------------

renderer::Settings JSONReader::ReadSettingsFromJSON(json::Dict& info) {
    renderer::Settings settings;
    if(info.count("width") != 0) {
        settings.width = info.at("width").AsDouble();
        
    }
    if(info.count("height") != 0) {
        settings.height = info.at("height").AsDouble();
    }
    if(info.count("padding") != 0) {
        settings.padding = info.at("padding").AsDouble();
    }
    if(info.count("line_width") != 0) {
        settings.line_width = info.at("line_width").AsDouble();
    }
    if(info.count("stop_radius") != 0) {
        settings.stop_radius = info.at("stop_radius").AsDouble();
    }
    if(info.count("bus_label_font_size") != 0) {
        settings.bus_lable_font_size = info.at("bus_label_font_size").AsInt();
        //std::cout << *settings.bus_lable_font_size << " = " << 
    }
    if(info.count("bus_label_offset") != 0) { 
        settings.bus_label_offset = {info.at("bus_label_offset").AsArray().at(0).AsDouble(), info.at("bus_label_offset").AsArray().at(1).AsDouble()}; 
    }
    if(info.count("stop_label_font_size") != 0) {
        settings.stop_label_font_size = info.at("stop_label_font_size").AsInt();
    }
    if(info.count("stop_label_offset") != 0) {
         settings.stop_label_offset = {info.at("stop_label_offset").AsArray().at(0).AsDouble(), info.at("stop_label_offset").AsArray().at(1).AsDouble()};
    }
    if(info.count("underlayer_color") != 0) {
        auto value = info.at("underlayer_color");
        if(value.IsString()) {
            settings.underlayer_color =(value.AsString());
        } else if(value.IsArray()) {
            if(value.AsArray().size() == 3) {
                settings.underlayer_color =(Rgb(value.AsArray().at(0).AsInt(), value.AsArray().at(1).AsInt(), value.AsArray().at(2).AsInt()));
            } else if (value.AsArray().size() == 4) {
                settings.underlayer_color =(Rgba(value.AsArray().at(0).AsInt(), value.AsArray().at(1).AsInt(), value.AsArray().at(2).AsInt(), value.AsArray().at(3).AsDouble()));
            }

        }
    }
    
    if(info.count("underlayer_width") != 0) {
        settings.underlayer_width = info.at("underlayer_width").AsDouble();
        
    }
    if(info.count("color_palette") != 0) {
        std::vector<svg::Color> v_color;
        for(auto& value : info.at("color_palette").AsArray()) {
            
            if(value.IsString()) {
                
                v_color.push_back(value.AsString());
            }
            
            if(value.IsArray()) {
                if(value.AsArray().size() == 3) {
                    v_color.push_back(Rgb(value.AsArray().at(0).AsInt(), value.AsArray().at(1).AsInt(), value.AsArray().at(2).AsInt()));
                } else if (value.AsArray().size() == 4) {
                    v_color.push_back(Rgba(value.AsArray().at(0).AsInt(), value.AsArray().at(1).AsInt(), value.AsArray().at(2).AsInt(), value.AsArray().at(3).AsDouble()));
                }
                
            }
        }
        settings.color_palette = v_color;
        
        
    }
    
    return settings;
}
    
Bus JSONReader::ReadBusFromJSON( json::Dict& info, catalogue::TransportCatalogue& catalogue) {
    
    Bus bus;
    
    bus.number = info["name"].AsString();
    for(const json::Node& value : info["stops"].AsArray()) {
        std::string stp = value.AsString();
        bus.stops.push_back(catalogue.FindStop(stp));
    }
    bus.round = info["is_roundtrip"].AsBool();
    if(!info["is_roundtrip"].AsBool()) {
        
        for(int i = static_cast<int>(bus.stops.size() - 2); i != -1; i--) {
            bus.stops.push_back(bus.stops[i]);
            
        }
    }
    bus.length = catalogue.CalculateLength(bus.stops);
    bus.dist = catalogue.CalculateDistance(bus.stops);
    return bus;
}

Stop JSONReader::ReadStopFromJSON(json::Dict& info) {
    Stop stop;
    
    stop.name = info["name"].AsString();
    stop.coordinates.lng = info["longitude"].AsDouble();
    stop.coordinates.lat = info["latitude"].AsDouble();
    
    for(auto& [stp, dis] : info["road_distances"].AsDict()) {
        TwoStops two_stops;
        two_stops.stop1 = stop.name;
        two_stops.stop2 = stp;
        stop.distances[two_stops] = dis.AsDouble();
    }
    return stop;
}

void JSONReader::ReadBaseRequests(json::Dict& dc, catalogue::TransportCatalogue& catalogue) {
    for(const auto& [key, value] : dc) {
        if(key == "base_requests") {
            for(const auto& val : value.AsArray() ) {
                if(val.AsDict().at("type").AsString() == "Stop") {
                    Dict dic = val.AsDict();
                    catalogue.AddStop(ReadStopFromJSON(dic));
                }
            }
            for(const auto& val : value.AsArray() ) {
                if(val.AsDict().at("type").AsString() == "Bus") {
                    Dict dic = val.AsDict();
                    catalogue.AddBus(ReadBusFromJSON(dic, catalogue));
                }
            }
        }
    }
}
    
void JSONReader::ReadSettingsRequests(json::Dict& dc, renderer::Settings& settings) {
    for( auto& [key, value] : dc) {
        if(key == "render_settings") {
            Dict dic = value.AsDict();
            settings = ReadSettingsFromJSON(dic);
           
        }
    }
}
    
void JSONReader::ReadStatRequests(json::Dict& dc, std::ostream& out, catalogue::TransportCatalogue& catalogue, renderer::Settings& settings) {
    stat::JSONPrinter printer;
    for(const auto& [key, value] : dc) {
        if(key == "stat_requests") {
            out << '[';
            bool first = true;
            
            for(const auto& val : value.AsArray() ) {
                if(first) {
                    first = false;
                } else {
                    out << ", ";
                }
                if(val.AsDict().at("type").AsString() == "Stop") {
                    
                    printer.PrintJSONStop(out, val.AsDict().at("name").AsString(), val.AsDict().at("id").AsInt(), catalogue);
                }
                
                if(val.AsDict().at("type").AsString() == "Bus") {
                    printer.PrintJSONBus(out, val.AsDict().at("name").AsString(), val.AsDict().at("id").AsInt(), catalogue);
                }
                
                if(val.AsDict().at("type").AsString() == "Map") {
                    printer.PrintMapJSON(out, val.AsDict().at("id").AsInt(), catalogue, settings );
                }
            }
            out << ']';
        }
    }
}
    



void JSONReader::ReadRequests(std::istream& input, [[maybe_unused]] std::ostream& out, catalogue::TransportCatalogue& catalogue) {
    const auto doc = json::Load(input);
    
    json::Dict dc = doc.GetRoot().AsDict();
   
    renderer::Settings settings;
    ReadBaseRequests(dc, catalogue);
    ReadSettingsRequests(dc, settings);
    ReadStatRequests(dc, out, catalogue, settings);

}
    
    
  


//-------------------------------END-IN-JSON_READER-----------------

json::Document LoadJSON(const std::string& s) {
    try {
    std::istringstream strm(s);
    return json::Load(strm);
    } catch(...) {
        throw ParsingError("err");
    }
}
    
    
InputCall ReadInput(std::string_view input) {
    InputCall inp_to_push;
    size_t pos = input.find(' ');
    inp_to_push.call = {&input[0], &input[pos]};
    inp_to_push.info = {&input[pos+1], &input[input.size()]};
    return inp_to_push;
}

Stop ReadStop(std::string_view input) {
    Stop stop;
    stop.name = {&input[0], find(input.begin(), input.end(), ':')};
    size_t strt = input.find(':') + 1;
    string lats, lngs;
    std::copy(&input[strt+1], &input[input.find(',', strt)], lats.begin());
    stop.coordinates.lat = stod(lats);
    strt = input.find(',', strt ) + 1;
    if (input.find(',', strt + 1) == std::string::npos) {
        lngs = {&input[strt + 1], &input[input.size()]};
        stop.coordinates.lng = stod(lngs);
    } else {
        lngs = {&input[strt + 1], &input[input.find(',', strt)]};
        stop.coordinates.lng = stod(lngs);
        do {
            strt = input.find(',', strt + 1) + 1;
            string s_dist = {&input[strt], &input[input.find('m', strt + 1)]};
            double dist;
            dist = stod(s_dist);
            strt = input.find("to", strt)  + 3;
            auto pos = input.size();
            if(input.find(',', strt + 1) != std::string::npos) {
                pos = input.find(',', strt + 1) ;
            }
            string second_name;
            second_name = {&input[strt], &input[pos]};
            while(second_name[second_name.size()-1] == ' ') {
                second_name.pop_back();
            }
            TwoStops two_stops1, two_stops2;
            two_stops1.stop1 = stop.name;
            two_stops1.stop2 = second_name;
            stop.distances[two_stops1] = dist;
        } while (input.find(',', strt+1) != std::string::npos);
    }
    return stop;
}

Bus ReadBus(std::string_view input, TransportCatalogue& catalogue) {
    Bus bus;
    string number;
    bus.number = {input.begin(), find(input.begin(), input.end(), ':')};
    size_t twodots = input.find(':') + 2;
    if(input.find('>') == std::string::npos) {
        string name;
        size_t n = input.find(" - ");
        name = {&input[twodots], &input[n]};
        bus.stops.push_back(catalogue.FindStop(name));
        name = "";
        size_t strt = n+3;
        n = input.find( " - ", n + 3);
        while(n != std::string::npos) {
            name = {&input[strt], &input[n]};
            bus.stops.push_back(catalogue.FindStop(name));
            name = "";
            strt = n+3;
            n = input.find(" - ", n + 3);
        }
        name = {&input[strt], &input[input.size()]};
        bus.stops.push_back(catalogue.FindStop(name));
        for(int i = static_cast<int>(bus.stops.size() - 2); i != -1; i--) {
            bus.stops.push_back(bus.stops[i]);
        }
        
        
        
    } else {
        string name;
        size_t n = input.find(" > ");
        name = {&input[twodots], &input[n]};
        bus.stops.push_back(catalogue.FindStop(name));
        name = "";
        size_t strt = n+3;
        n = input.find(" > ",n+3);
        while(n != std::string::npos) {
            name = {&input[strt], &input[n]};
            bus.stops.push_back(catalogue.FindStop(name));
            name = "";
            strt = n+3;
            n = input.find(" > ", strt);
        }
        name = {&input[strt], &input[input.size()]};
        bus.stops.push_back(catalogue.FindStop(name));
    }
    bus.length = catalogue.CalculateLength(bus.stops);
    bus.dist = catalogue.CalculateDistance(bus.stops);

    return bus;
}

void InsertBase( TransportCatalogue& catalogue, istream& input_stream) {
    int mn;
    string input;
    input_stream >> mn;
    vector<InputCall> inpcalvec;
    getline(input_stream, input);
    for(int i = 0; i < mn; ++i) {
        getline(input_stream, input);
        inpcalvec.push_back((ReadInput(input)));
    }
    for(InputCall a : inpcalvec) {
        if(a.call == "Stop") {
            catalogue.AddStop((ReadStop(move(a.info))));
        }
    }
    for(InputCall a : inpcalvec) {
        if(a.call == "Bus") {
            catalogue.AddBus(ReadBus(move(a.info), catalogue));
        }
    }
}
}
