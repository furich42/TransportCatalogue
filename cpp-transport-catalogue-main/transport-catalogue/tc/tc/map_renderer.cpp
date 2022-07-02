#include "map_renderer.h"

bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

    // Проецирует широту и долготу в координаты внутри SVG-изображения
svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return {
        (coords.lng - min_lon_) * zoom_coeff_ + padding_,
        (max_lat_ - coords.lat) * zoom_coeff_ + padding_
    };
}

SphereProjector SetProjector(std::vector<domain::Bus*> buses, double a, double b, double c) {
            std::vector<geo::Coordinates> coords;
                for(auto bus : buses) {
                    for(auto stop : bus->stops) {
                        coords.push_back(stop->coordinates);
                    }
                }
               return  SphereProjector{coords.begin(), coords.end(), a, b, c};
            
                
        }


namespace renderer {
   
    void MapRenderer::FormPol(domain::Bus* bus, int i) {
        svg::Polyline way;

        std::vector<svg::Point> points;
        for(auto stop : bus->stops) {

            way.AddPoint(projector(stop->coordinates));
        }
        doc.Add(svg::Polyline{way}.SetStrokeColor((*settings.color_palette).at(i)).SetStrokeWidth(*settings.line_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetFillColor(NoneColor));
    }
    
    void MapRenderer::FormBusName(domain::Stop* stop, std::string name, svg::Color color) {
        svg::Text under_text = Text()
            .SetFontFamily("Verdana")
            .SetFontSize(*settings.bus_lable_font_size)
            .SetPosition(projector(stop->coordinates))
            .SetData(name)
            .SetOffset(*settings.bus_label_offset)
            .SetFontWeight("bold");
        svg::Text text = Text()
            .SetFontFamily("Verdana")
            .SetFontSize(*settings.bus_lable_font_size)
            .SetPosition(projector(stop->coordinates))
            .SetData(name)
            .SetOffset(*settings.bus_label_offset)
            .SetFontWeight("bold");

        doc.Add(svg::Text{under_text}.SetFillColor(*settings.underlayer_color).SetStrokeColor(*settings.underlayer_color).SetStrokeWidth(*settings.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
    doc.Add(svg::Text{text}.SetFillColor(color));
    }
    
    void MapRenderer::FormStop(domain::Stop* stop) {
        svg::Circle circle;
        circle.SetCenter(projector(stop->coordinates)).SetRadius(*settings.stop_radius);
        doc.Add(svg::Circle{circle}.SetFillColor(Color{"white"}));
    }
    
    void MapRenderer::FormStopName(domain::Stop* stop) {
        svg::Text under_text = Text()
            .SetFontFamily("Verdana")
            .SetFontSize(*settings.stop_label_font_size)
            .SetPosition(projector(stop->coordinates))
            .SetData(stop->name)
            .SetOffset(*settings.stop_label_offset);
        svg::Text text = Text()
            .SetFontFamily("Verdana")
            .SetFontSize(*settings.stop_label_font_size)
            .SetPosition(projector(stop->coordinates))
            .SetData(stop->name)
            .SetOffset(*settings.stop_label_offset);

        doc.Add(svg::Text{under_text}.SetFillColor(*settings.underlayer_color).SetStrokeColor(*settings.underlayer_color).SetStrokeWidth(*settings.underlayer_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
    doc.Add(svg::Text{text}.SetFillColor(Color{"black"}));
    }
        
    std::ostringstream& MapRenderer::FormMap(std::ostringstream& out) {
        int i = 0;

            

        for(domain::Bus* bus : buses) {
            if(bus->stops.size() > 1) {
                FormPol(bus, i); 
                if(++i == static_cast<int>((*settings.color_palette).size())) {
                    i = 0;
                }
            }

        }
        i = 0;
        for(domain::Bus* bus : buses) {
            if(bus->stops.size() > 1) {
                FormBusName(bus->stops.at(0), bus->number, (*settings.color_palette).at(i));
                if((!bus->round) && bus->stops.at(bus->stops.size()/2) != bus->stops.at(0)) {
                    FormBusName(bus->stops.at(bus->stops.size()/2), bus->number, (*settings.color_palette).at(i));
                }
                if(++i == static_cast<int>((*settings.color_palette).size())) {
                    i = 0;
                }
            }

        }
        
        

        for(domain::Stop* stop : stops) {
            FormStop(stop);
        }  

        for(domain::Stop* stop : stops) {
            FormStopName(stop);
        }
        doc.Render(out);
        return out;
    }
    
    
    
    
    
}