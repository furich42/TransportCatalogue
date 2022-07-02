#pragma once

#include <optional>

#include "svg.h"
#include "geo.h"
#include "domain.h"
#include <vector>

#include <algorithm>
#include <cstdlib>
#include <iostream>


/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
using namespace svg;

inline const double EPSILON = 1e-6;
bool IsZero(double value);

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    SphereProjector() = default;
    
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const ;

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

SphereProjector SetProjector(std::vector<domain::Bus*> buses, double a, double b, double c);
        

namespace renderer {
    
    struct Settings {
        std::optional<double> width = std::nullopt;
        std::optional<double> height = std::nullopt;
        
        std::optional<double> padding = std::nullopt;
        std::optional<double> line_width = std::nullopt;
        std::optional<double> stop_radius = std::nullopt;
        std::optional<int> bus_lable_font_size = std::nullopt;
        std::optional<Point> bus_label_offset = std::nullopt;
        std::optional<int> stop_label_font_size = std::nullopt;
        std::optional<Point> stop_label_offset = std::nullopt;
        std::optional<Color> underlayer_color = std::nullopt;
        std::optional<double> underlayer_width = std::nullopt;
        std::optional<std::vector<Color>> color_palette = std::nullopt;
        
    };
    
    class MapRenderer {
    public:
        
        explicit MapRenderer(Settings& settings_, std::vector<domain::Bus*> buses, std::vector<domain::Stop*> stops)
            :settings(settings_), buses(buses), stops(stops) {projector = SetProjector(buses, *settings.width, *settings.height, *settings.padding);}
        
        
        void FormPol(domain::Bus* bus, int i);
        void FormBusName(domain::Stop* stop, std::string name, svg::Color color);
        void FormStop(domain::Stop* stop);
        void FormStopName(domain::Stop* stop);
        std::ostringstream& FormMap(std::ostringstream& out);

    private:
        Settings& settings;
        svg::Document doc;
        std::vector<domain::Bus*> buses; 
        std::vector<domain::Stop*> stops;  
        SphereProjector projector;
    };
    
   
    
    
}