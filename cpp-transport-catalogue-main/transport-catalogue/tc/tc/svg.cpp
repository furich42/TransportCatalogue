#include "svg.h"

namespace svg {

using namespace std::literals;
    
    
    
//-------------Color-----------------
    
    
    

    
void ColorPrinter::operator()(std::monostate) const {
    out << "none"sv;
}
void ColorPrinter::operator()(std::string colorr) const {
    out << colorr;
}
void ColorPrinter::operator()(Rgb rgb) const {
    out << "rgb("sv << +rgb.red << ","sv << +rgb.green << ","sv << +rgb.blue << ")"sv;
}
void ColorPrinter::operator()(Rgba rgba) const {
    if(rgba.opacity > 1.0) {
        rgba.opacity = 1.0;
    } else if (rgba.opacity < 0.0) {
        rgba.opacity = 0.0;
    }
    out << "rgba("sv << +rgba.red << ","sv << +rgba.green << ","sv << +rgba.blue << ","sv << +rgba.opacity << ")"sv;
}
    
    
std::ostream& operator<<(std::ostream& out, svg::Color color) {
    std::visit(ColorPrinter{out}, color);
    return out;
}    
    
//--------------end-of-Color ---------------
    
   



void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}
    
    
    
    
std::ostream& operator<<(std::ostream& out, StrokeLineCap slc) {
    switch (slc)
    {
        case StrokeLineCap::BUTT:
            return out << "butt";
        case StrokeLineCap::ROUND:
            return out << "round";
        case StrokeLineCap::SQUARE:
            return out << "square";
        default:
            return out << "";
    }
        
}
    
std::ostream& operator<<(std::ostream& out, StrokeLineJoin slj) {
    switch (slj)
    {
        case StrokeLineJoin::ARCS:
            return out << "arcs";
        case StrokeLineJoin::BEVEL:
            return out << "bevel";
        case StrokeLineJoin::MITER:
            return out << "miter";
        case StrokeLineJoin::MITER_CLIP:
            return out << "miter-clip";
        case StrokeLineJoin::ROUND:
            return out << "round";
        default:
            return out << "";
    }
        
}
    
    
    
//---------------------------------------    
    
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

    // Выводит в ostream svg-представление документа
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    int i = 2;
    for(const std::unique_ptr<svg::Object>& obj : objects_) {
        RenderContext ctx(out, i++, 2);
        obj->Render(ctx);
    }
    out << "</svg>"sv;
}   
    
    
  
    



// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    
    
    RenderAttrs(out);
    
    
    out << "/>"sv;
}
    
    
    
// --------- Polyline -------------
    
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool first = true;
    for(auto& point : points_) {
        if (first) {
            out << point.x << ","sv << point.y;
            first = false;
        } else {
            out << " "sv << point.x << ","sv << point.y;
        }
        
    }
    
    out << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
    
}



    
    
    
// --------- Text --------------
    
// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
    
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    for(char c : data) {
        if(c == '\"') {
            data_ += "&quot;"sv;
        } else if(c == '\'') {
            data_ += "&apos;"sv;
        } else if(c =='<') {
            data_ += "&lt;"sv;
        } else if (c == '>') {
            data_ += "&gt;"sv;
        } else if (c == '&') {
            data_ += "&amp;"sv;
        } else {
            data_ += c;
        }
    }

    return *this;
}
    
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text"sv;
    RenderAttrs(out);
    out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\""sv;
    out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
    out << " font-size=\""sv << size_  << "\""sv;
    if(!font_family_.empty()) {
        out << " font-family=\""sv << font_family_  << "\""sv;
    }
    if(!font_weight_.empty()) {
        out << " font-weight=\""sv << font_weight_  << "\""sv;
    }
    out << ">"sv;
    if(!data_.empty()) {
        out << data_;
    }
    out << "</text>"sv;
    
}
  
}  // namespace svg
