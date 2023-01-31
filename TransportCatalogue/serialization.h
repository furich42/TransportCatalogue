#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "transport_catalogue.pb.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <variant>

namespace Serialize {

struct SerializeSettings {
    std::string file;
};

class Serializer {

public:
    Serializer() = delete;
    Serializer(const catalogue::TransportCatalogue& catalogue,const catalogue::TransportRouter& transport_router, const renderer::RenderSettings& render_settings,const SerializeSettings serialization_settings, const graph::Router<BusRouteWeight>& router);

    void SaveTo(const std::filesystem::path& path) const ;

    void Save() const ;
    
private:
    const catalogue::TransportCatalogue& catalogue_;
    const renderer::RenderSettings& render_settings_;
    const SerializeSettings serialize_settings_;
    const catalogue::TransportRouter& transport_router_;
    const graph::Router<BusRouteWeight>& router_;
    tc_pb::TransportBase pb_base_;


    struct RenderSettingsColorVisitor {
        tc_pb::Color& pb_color;

        void operator()(std::monostate) {
            pb_color.set_is_initialized(false);
        }
        void operator()(const std::string& color) {
            *pb_color.mutable_string_color() = color;
            pb_color.set_is_initialized(true);
        }
        void operator()(svg::Rgb rgb) {
            pb_color.mutable_rgb_color()->set_red(rgb.red);
            pb_color.mutable_rgb_color()->set_green(rgb.green);
            pb_color.mutable_rgb_color()->set_blue(rgb.blue);
            pb_color.set_is_initialized(true);
        } 
        void operator()(svg::Rgba rgba) {
            pb_color.mutable_rgba_color()->set_red(rgba.red);
            pb_color.mutable_rgba_color()->set_green(rgba.green);
            pb_color.mutable_rgba_color()->set_blue(rgba.blue);
            pb_color.mutable_rgba_color()->set_opacity(rgba.opacity);
            pb_color.set_is_initialized(true);
        } 
    };
    
    void FillRenderSettings();

    void FillRoutingSettings();

    void FillTransportRouter();

    void FillRouter();

};

class Deserializer {
public:
    Deserializer() = delete;
    Deserializer(const std::filesystem::path& p);
    Deserializer(SerializeSettings settings);

    catalogue::TransportCatalogue GetTransportCatalogue() const;
    catalogue::RoutingSettings GetRoutingSettings() const;
    renderer::RenderSettings GetRenderSettings() const;

    catalogue::TransportRouter GetTransportRouter(const catalogue::TransportCatalogue& catalogue) const;
    graph::Router<BusRouteWeight> GetRouter(const graph::DirectedWeightedGraph<BusRouteWeight>& graph) const;
private:
    std::filesystem::path open_path_;

    tc_pb::TransportBase pb_base_;
    SerializeSettings serialize_settings_;

    static svg::Color ExtractSVGColorFromPBColor(tc_pb::Color pb_color);
};

} // namespace Serialize
