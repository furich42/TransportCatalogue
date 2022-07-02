#pragma once

 

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <ostream>
#include <variant>
#include <sstream>
#include <cassert>

namespace svg {
    
    
enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};
    
std::ostream& operator<<(std::ostream& out, StrokeLineCap slc);
std::ostream& operator<<(std::ostream& out, StrokeLineJoin slj);
    

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

    
struct Rgb {
    
    Rgb() = default;
    Rgb(int r, int g, int b) 
        :red(r), green(g), blue(b){
        
    }
    
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0 ;
    
};
    
struct Rgba {
    
    Rgba() = default;
    
    Rgba(int r, int g, int b, double o) 
        :red(r), green(g), blue(b), opacity(o){
        
    }
    
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
    
};

using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

inline const Color NoneColor{"none"};
    
std::ostream& operator<<(std::ostream& out, svg::Color colorr) ;


    
struct ColorPrinter {
    
std::ostream& out;
    
void operator()(std::monostate) const ;
void operator()(std::string color) const;
void operator()(Rgb rgb) const;
void operator()(Rgba rgba) const;
     
    
    
    
};
    
    
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }
    
    //void RenderAttrs(std::ostream& out);

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

template <typename Owner>
class PathProps {
public:

    Owner& SetFillColor(Color color) {
        f_c_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        s_c_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        s_w_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        sl_c_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        sl_j_ = line_join;
        return AsOwner();
    }

   
    
protected:
    std::optional<svg::Color> f_c_ = std::nullopt;
    std::optional<svg::Color> s_c_ = std::nullopt;
    std::optional<double> s_w_ = std::nullopt;
    std::optional<svg::StrokeLineCap> sl_c_ = std::nullopt;
    std::optional<svg::StrokeLineJoin> sl_j_ = std::nullopt;
    
    void RenderAttrs(std::ostream& out) const {
    if(f_c_ != std::nullopt) {
        out << " fill=" << "\"" << *f_c_ <<"\"";
    }
    if(s_c_ != std::nullopt) {
        out << " stroke=" << "\"" << *s_c_ <<"\"";
    }
    if(s_w_ != std::nullopt) {
        out << " stroke-width=" << "\"" << *s_w_ <<"\"";
    }
    if(sl_c_ != std::nullopt) {
        out << " stroke-linecap=" << "\"" << *sl_c_ <<"\"";
    }
    if(sl_j_ != std::nullopt) {
        out << " stroke-linejoin=" << "\"" << *sl_j_ <<"\"";
    }
}
    
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }
    
    ~PathProps() = default;
    
    
    
    
};
    
class Object {
public:
    void Render(const RenderContext& context) const;
    

    virtual ~Object() = default;

protected:
    virtual void RenderObject(const RenderContext& context) const = 0;
    
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;
    Point center_;
    double radius_ = 1.0;
};

class Polyline : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;
protected:
    std::vector<Point> points_;
};

class Text : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    // Прочие данные и методы, необходимые для реализации элемента <text>

    
private:
    void RenderObject(const RenderContext& context) const override;
    
    Point pos_ = {0.0,0.0};
    Point offset_ = {0.0,0.0};
    uint32_t size_ = 1;
    std::string font_family_ = "";
    std::string font_weight_ = "";
    std::string data_ = "";
};
    
class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }
    
     
    
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
protected:
    std::vector<std::unique_ptr<svg::Object>> objects_;
    
    ~ObjectContainer() = default;
};

class Document : public ObjectContainer {
public:
   
     
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // Прочие методы и данные, необходимые для реализации класса Document

};
    

    
class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    
    virtual ~Drawable() = default;
};
    

    
   

}  // namespace svg