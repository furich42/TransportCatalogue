#include "serialization.h"

namespace Serialize
{


void FillStops(tc_pb::TransportCatalogue& pb_catalogue_, const catalogue::TransportCatalogue& catalogue) {
    for (const auto& stop : catalogue.GetStops()) {
        tc_pb::Stop pb_stop;
        pb_stop.set_id(stop.id);
        pb_stop.set_name(stop.name_);
        pb_stop.mutable_coordinates()->set_lat(stop.cordinates_.lat);
        pb_stop.mutable_coordinates()->set_lng(stop.cordinates_.lng);
        pb_catalogue_.mutable_stops()->Add(std::move(pb_stop));
    }
}


void FillBuses(tc_pb::TransportCatalogue& pb_catalogue_, const catalogue::TransportCatalogue& catalogue) {
    for(const auto& bus : catalogue.GetBuses()) {
        tc_pb::Bus pb_bus;
        pb_bus.set_id(bus.id);
        pb_bus.set_name(bus.name_);
        
        for(const auto& stop : bus.stops_) {
            pb_bus.add_stops(stop->id);
        }
        switch (bus.bus_type_)
        {
        case BusType::ORDINARY:
            pb_bus.set_bus_type_cycled(false);
            break;
        case BusType::CYCLED:
            pb_bus.set_bus_type_cycled(true);
            break;
        default:
            break;
        }
        pb_catalogue_.mutable_buses()->Add(std::move(pb_bus));
    }
}

void FillStopsToBusses(tc_pb::TransportCatalogue& pb_catalogue_, const catalogue::TransportCatalogue& catalogue) {
    for(const auto& stop_to_buses : catalogue.GetStopsToBuses()) {
        tc_pb::StopToBuses pb_stop_to_buses;
        
        int stop_id = stop_to_buses.first->id;
        pb_stop_to_buses.set_stop_id(stop_id);

        const std::set<const Bus*>& buses = stop_to_buses.second;
        for(const auto& bus : buses) {
            pb_stop_to_buses.mutable_bus_id()->Add(bus->id);
        }
        pb_catalogue_.mutable_stops_to_buses()->Add(std::move(pb_stop_to_buses));
    }
}

void FillIntervals(tc_pb::TransportCatalogue& pb_catalogue_, const catalogue::TransportCatalogue& catalogue) {
    for(const auto& interval_to_distance : catalogue.GetIntervalsToDistance()) {
        tc_pb::IntervalToDistance pb_interval_to_distance;
        int from_id = interval_to_distance.first.first->id;
        int to_id = interval_to_distance.first.second->id;
        int64_t distance = interval_to_distance.second;
        pb_interval_to_distance.set_from_id(from_id);
        pb_interval_to_distance.set_to_id(to_id);
        pb_interval_to_distance.set_distance(distance);
        pb_catalogue_.mutable_intervals_to_distance()->Add(std::move(pb_interval_to_distance));
    }
}


Serializer::Serializer(const catalogue::TransportCatalogue& catalogue,
                       const catalogue::TransportRouter& transport_router,
                       const renderer::RenderSettings& render_settings,
                       const SerializeSettings serialization_settings,
                       const graph::Router<BusRouteWeight>& router)
               : catalogue_(catalogue)
               , render_settings_(render_settings)
               , serialize_settings_(serialization_settings)
               , transport_router_(transport_router)
               , router_(router)
           {
               tc_pb::TransportCatalogue pb_catalogue_;
               
               FillStops(pb_catalogue_, catalogue_);
               FillBuses(pb_catalogue_, catalogue_);
               FillStopsToBusses(pb_catalogue_, catalogue_);
               FillIntervals(pb_catalogue_, catalogue_);
               
               

               *pb_base_.mutable_cat() = std::move(pb_catalogue_);
                   
              // FillRoutingSettings();
               FillRenderSettings();
               FillTransportRouter();
               FillRouter();
           }




void Serializer::SaveTo(const std::filesystem::path& path) const {
    std::ofstream out(path, std::ios::binary);

    pb_base_.SerializeToOstream(&out);
}

void Serializer::Save() const {
    std::ofstream out(std::filesystem::path(serialize_settings_.file), std::ios::binary);
    pb_base_.SerializeToOstream(&out);
}

void Serializer::FillRenderSettings() {
    tc_pb::RenderSettings pb_render_settings;
    pb_render_settings.set_width(render_settings_.width);
    pb_render_settings.set_height(render_settings_.height);
    pb_render_settings.set_padding(render_settings_.padding);
    pb_render_settings.set_line_width(render_settings_.line_width);
    pb_render_settings.set_stop_radius(render_settings_.stop_radius);
    pb_render_settings.set_bus_label_font_size(render_settings_.bus_label_font_size);
    pb_render_settings.mutable_bus_label_offset()->set_x(render_settings_.bus_label_offset.x);
    pb_render_settings.mutable_bus_label_offset()->set_y(render_settings_.bus_label_offset.y);
    pb_render_settings.set_stop_label_font_size(render_settings_.stop_label_font_size);
    pb_render_settings.mutable_stop_label_offset()->set_x(render_settings_.stop_label_offset.x);
    pb_render_settings.mutable_stop_label_offset()->set_y(render_settings_.stop_label_offset.y);

    tc_pb::Color pb_color;
    visit(RenderSettingsColorVisitor{pb_color}, render_settings_.underlayer_color);
    *pb_render_settings.mutable_underlayer_color() = pb_color;

    pb_render_settings.set_underlayer_width(render_settings_.underlayer_width);

    for(const auto& color : render_settings_.color_palette) {
        tc_pb::Color pb_color;
        visit(RenderSettingsColorVisitor{pb_color}, color);
        pb_render_settings.mutable_color_palette()->Add(std::move(pb_color));
    }

    *pb_base_.mutable_rendder_settings() = std::move(pb_render_settings);
}

void Serializer::FillTransportRouter() {
    tc_pb::TransportRouter pb_transport_router;

    const graph::DirectedWeightedGraph<BusRouteWeight>& graph = transport_router_.GetRouteGraph<BusRouteWeight>();
    for(const auto& edge : graph.GetEdges()) {
        tc_pb::Edge pb_edge;
        pb_edge.set_vertex_id_from(edge.from);
        pb_edge.set_vertex_id_to(edge.to);

        tc_pb::BusRouteWeight pb_weight;
        pb_weight.set_span(edge.weight.span);
        pb_weight.set_time(edge.weight.time);

        *pb_edge.mutable_weight() = pb_weight;

        pb_transport_router.mutable_route_graph()->mutable_edges()->Add(std::move(pb_edge));
    }

    for(const std::vector<graph::EdgeId>& incidence_list : graph.GetIncidenceLists()) {
        tc_pb::IncidenceList pb_incidence_list;

        for(graph::EdgeId edge_id : incidence_list) {
            pb_incidence_list.add_edge_id(edge_id);
        }

        pb_transport_router.mutable_route_graph()->mutable_incidence_lists()->Add(std::move(pb_incidence_list));
    }

    for(const Stop* stop : transport_router_.GetVertexIndexToStop()) {
        pb_transport_router.add_vertex_index_to_stop(stop->id);
    }
    for(const Bus* bus : transport_router_.GetEdgeIndexToBus()) {
        if(bus) {
            tc_pb::BusId pb_bus_id;
            pb_bus_id.set_bus_id(bus->id);
            pb_bus_id.set_isinitialized(true);
            pb_transport_router.mutable_edge_index_to_bus()->Add(std::move(pb_bus_id));

        } else {
            tc_pb::BusId pb_bus_id;
            pb_bus_id.set_isinitialized(false);
            pb_transport_router.mutable_edge_index_to_bus()->Add(std::move(pb_bus_id));
            
        }
    }
    *pb_base_.mutable_transport_router() = std::move(pb_transport_router);
}

void Serializer::FillRouter() {
    
    using RouteInternalData = graph::Router<BusRouteWeight>::RouteInternalData;

    tc_pb::Router pb_router;

    for (const std::vector<std::optional<RouteInternalData>>& row : router_.GetRoutesInternalData()) {
        
        tc_pb::RouteInternalDataRow pb_row;

        for(const std::optional<RouteInternalData>& data : row) {
            if(data.has_value()) {
                tc_pb::RouteInternalData pb_data;

                pb_data.mutable_weight()->set_span(data->weight.span);
                pb_data.mutable_weight()->set_time(data->weight.time);

                if(data->prev_edge.has_value()) {
                    pb_data.mutable_prev_edge()->set_prev_edge_id(data->prev_edge.value());
                }

                pb_row.mutable_route_internal_data_row()->Add(std::move(pb_data));

            } else {
                pb_row.add_route_internal_data_row();
            }
        }

        pb_router.mutable_routes_internal_data()->Add(std::move(pb_row));
    }

    *pb_base_.mutable_router() = std::move(pb_router);
}

Deserializer::Deserializer(const std::filesystem::path& p)
: open_path_(p)
    {
    std::ifstream input_file(open_path_, std::ios::binary);
    if(!input_file) {
        throw std::runtime_error("Can't open file? bad path?");
    }

    pb_base_.ParseFromIstream(&input_file);
    }

Deserializer::Deserializer(SerializeSettings settings)
    : open_path_(std::filesystem::path(settings.file))
    , serialize_settings_(settings)

    {
    std::ifstream input_file(open_path_, std::ios::binary);
    if(!input_file) {
        throw std::runtime_error("Can't open file? bad path?");
    }

pb_base_.ParseFromIstream(&input_file);
}

void StopsGetter(catalogue::TransportCatalogue& result, const tc_pb::TransportCatalogue& pb_catalogue) {
    std::deque<Stop> stops;
    std::map<std::string_view, const Stop*> stopname_to_stop;
    for(const auto& pb_stop : pb_catalogue.stops()) {
        Stop& emplaced = stops.emplace_back(
            Stop{
                std::string(pb_stop.name()),
                geo::Coordinates{
                    pb_stop.coordinates().lat(),
                    pb_stop.coordinates().lng()
                },
                pb_stop.id()
            }
        );

        stopname_to_stop[std::string_view(emplaced.name_)] = &emplaced;
    }

    result.SetStops(std::move(stops));
    result.SetStopnameToStop(std::move(stopname_to_stop));
}

void BusesGetter(catalogue::TransportCatalogue& result, const tc_pb::TransportCatalogue& pb_catalogue) {
    std::deque<Bus> buses;
    std::map<std::string_view, const Bus*> busname_to_bus;
    for(const auto& pb_bus : pb_catalogue.buses()) {
        std::vector<const Stop*> stops;
        for(int stop_id : pb_bus.stops()) {
            assert(result.GetStops().at(stop_id).id == stop_id);
            stops.push_back(&(result.GetStops().at(stop_id)));
        }
        
        BusType bus_type;
        if(pb_bus.bus_type_cycled()) {
            bus_type = BusType::CYCLED;
        } else {
            bus_type = BusType::ORDINARY;
        }
        Bus current_bus{
            std::string(pb_bus.name()),
            std::move(stops),
            bus_type,
            pb_bus.id()
        };
        Bus& emlplaced = buses.emplace_back(std::move(current_bus));

        busname_to_bus[std::string_view(emlplaced.name_)] = &(emlplaced);
    }

    result.SetBuses(std::move(buses));
    result.SetBusnameToBus(std::move(busname_to_bus));
}

void StopsToBusesGetter(catalogue::TransportCatalogue& result, const tc_pb::TransportCatalogue& pb_catalogue) {
    std::unordered_map<const Stop*, std::set<const Bus*>> stops_to_buses;

    for(const auto& stop_to_buses : pb_catalogue.stops_to_buses()) {
        int stop_id = stop_to_buses.stop_id();
        const Stop* stop_ptr = &(result.GetStops().at(stop_id));
        assert(stop_id == stop_ptr->id);
        std::set<const Bus*> current_stop_buses;
        for(int bus_id : stop_to_buses.bus_id()) {
            const Bus* bus_ptr = &(result.GetBuses().at(bus_id));
            assert(bus_id == bus_ptr->id);
            stops_to_buses[stop_ptr].insert(bus_ptr);
        }
    }

    result.SetStopsToBuses(std::move(stops_to_buses));
}

void IntervalToDistancesGetter(catalogue::TransportCatalogue& result, const tc_pb::TransportCatalogue& pb_catalogue) {
    std::unordered_map<std::pair<const Stop*, const Stop*>, uint64_t, catalogue::RouteDistanceHasher> intervals_to_distance;

    for(const auto& interval : pb_catalogue.intervals_to_distance()) {
        int from_id = interval.from_id();
        int to_id = interval.to_id();
        int64_t distance = interval.distance();

        const Stop* stop_ptr_from = &(result.GetStops().at(from_id));
        assert(from_id == stop_ptr_from->id);
        const Stop* stop_ptr_to = &(result.GetStops().at(to_id));
        assert(to_id == stop_ptr_to->id);

        intervals_to_distance[{stop_ptr_from, stop_ptr_to}] = distance;
    }

    result.SetIntervalsToDistance(std::move(intervals_to_distance));
}

catalogue::TransportCatalogue Deserializer::GetTransportCatalogue() const {
    catalogue::TransportCatalogue result;
   
    const tc_pb::TransportCatalogue& pb_catalogue = pb_base_.cat();
    
    StopsGetter(result, pb_catalogue);
    BusesGetter(result, pb_catalogue);
    StopsToBusesGetter(result, pb_catalogue);
    IntervalToDistancesGetter(result, pb_catalogue);
    
    return result;
}

catalogue::RoutingSettings Deserializer::GetRoutingSettings() const {
    catalogue::RoutingSettings result;
    result.bus_velocity = pb_base_.routing_settings().bus_velocity();
    result.bus_wait_time = pb_base_.routing_settings().bus_wait_time();
    return result;
}

renderer::RenderSettings Deserializer::GetRenderSettings() const {
    renderer::RenderSettings result;
    const tc_pb::RenderSettings& pb_render_settings = pb_base_.rendder_settings();
    result.width = pb_render_settings.width();
    result.height = pb_render_settings.height();
    result.padding = pb_render_settings.padding();
    result.line_width = pb_render_settings.line_width();
    result.stop_radius = pb_render_settings.stop_radius();

    result.bus_label_font_size = pb_render_settings.bus_label_font_size();
    result.bus_label_offset = {pb_render_settings.bus_label_offset().x(), pb_render_settings.bus_label_offset().y()};

    result.stop_label_font_size = pb_render_settings.stop_label_font_size();
    result.stop_label_offset = {
        pb_render_settings.stop_label_offset().x(),
        pb_render_settings.stop_label_offset().y(),
    };

    result.underlayer_color = ExtractSVGColorFromPBColor(pb_render_settings.underlayer_color());
    result.underlayer_width = pb_render_settings.underlayer_width();

    for(const auto& pb_color : pb_render_settings.color_palette()) {
        result.color_palette.push_back(ExtractSVGColorFromPBColor(pb_color));
    }

    return result;
}

svg::Color Deserializer::ExtractSVGColorFromPBColor(tc_pb::Color pb_color) {
    svg::Color result;

    if(pb_color.is_initialized()) {
        if(pb_color.has_rgb_color()) {

            result = svg::Rgb{
                static_cast<uint8_t>(pb_color.rgb_color().red()), 
                static_cast<uint8_t>(pb_color.rgb_color().green()), 
                static_cast<uint8_t>(pb_color.rgb_color().blue())
            };
        } else if(pb_color.has_rgba_color()) {

            result = svg::Rgba{
                static_cast<uint8_t>(pb_color.rgba_color().red()), 
                static_cast<uint8_t>(pb_color.rgba_color().green()), 
                static_cast<uint8_t>(pb_color.rgba_color().blue()), 
                pb_color.rgba_color().opacity()
            };
        } else {
            result = pb_color.string_color();
        }
    }

    return result;
}

catalogue::TransportRouter 
Deserializer::GetTransportRouter(const catalogue::TransportCatalogue& catalogue) const {
    catalogue::TransportRouter result(GetRoutingSettings(), catalogue);
    
    std::deque<const Stop*> vertex_index_to_stop;
    std::map<std::string_view, graph::VertexId> stopname_to_vertex_id;

    for(const int32_t stop_id : pb_base_.transport_router().vertex_index_to_stop()) {
        const Stop*& emplaced = vertex_index_to_stop.emplace_back(&catalogue.GetStops().at(stop_id));
        stopname_to_vertex_id[emplaced->name_] = 2 * stop_id;
    }

    std::deque<const Bus*> edge_index_to_bus;
    for(auto bus_id : pb_base_.transport_router().edge_index_to_bus()) {
        if(bus_id.isinitialized()) {
            edge_index_to_bus.emplace_back(&catalogue.GetBuses().at(bus_id.bus_id()));
        } else {
            edge_index_to_bus.emplace_back(nullptr);
        }
    }

    result.SetVertexIndexToStop(std::move(vertex_index_to_stop));
    result.SetStopnameToVertexId(std::move(stopname_to_vertex_id));
    result.SetEdgeIndexToBus(std::move(edge_index_to_bus));

    graph::DirectedWeightedGraph<BusRouteWeight> route_graph(vertex_index_to_stop.size());
    for(const auto& pb_edge : pb_base_.transport_router().route_graph().edges()) {
        route_graph.AddEdge(
            graph::Edge<BusRouteWeight>{
                pb_edge.vertex_id_from(),
                pb_edge.vertex_id_to(),
                BusRouteWeight{
                    pb_edge.weight().time(),
                    pb_edge.weight().span()
                }
            }
        );
    }
    result.SetRouteGraph(std::move(route_graph));

    return result;
}

graph::Router<BusRouteWeight> Deserializer::GetRouter(const graph::DirectedWeightedGraph<BusRouteWeight>& graph) const {
    
    graph::Router<BusRouteWeight>::RoutesInternalData routes_internal_data;
    routes_internal_data.resize(graph.GetVertexCount());

    const tc_pb::Router& pb_router = pb_base_.router();
    size_t from_index = 0;
    for(const tc_pb::RouteInternalDataRow pb_route_internal_data_row : pb_router.routes_internal_data()) {
        routes_internal_data[from_index].resize(graph.GetVertexCount());
        size_t to_index = 0;
        for(const tc_pb::RouteInternalData& pb_route_internal_data : pb_route_internal_data_row.route_internal_data_row()) {
            if(pb_route_internal_data.has_weight()) {
                routes_internal_data[from_index][to_index] = {
                    {pb_route_internal_data.weight().time(), pb_route_internal_data.weight().span()},
                    std::nullopt
                };
                if(pb_route_internal_data.has_prev_edge()) {
                    routes_internal_data[from_index][to_index].value().prev_edge = pb_route_internal_data.prev_edge().prev_edge_id();
                }
            }

            ++to_index;
        }

        ++from_index;
    }
    
    graph::Router<BusRouteWeight> result(graph, std::move(routes_internal_data));

    return result;
}

} // namespace Serialize

