#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>
#include <variant>
#include <math.h>

namespace svg {

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
    Rgb(uint8_t r, uint8_t g, uint8_t b)
        : red(r), green(g), blue(b) {}
    Rgb() = default;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba {
    Rgba(uint8_t r, uint8_t g, uint8_t b, double a)
        : red(r), green(g), blue(b), opacity(a) {}
    Rgba() = default;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t);

std::ostream& operator<<(std::ostream& out, const Point& point);

using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

inline const Color NoneColor{"none"};

struct OstreamColorPrinter {
    std::ostream& out;
    void operator()(std::monostate);
    void operator()(const std::string& color);
    void operator()(const Rgb color);
    void operator()(const Rgba color);
};

std::ostream& operator<<(std::ostream& out, const Color& color);

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

std::ostream& operator<<(std::ostream& out, StrokeLineCap linecap);
std::ostream& operator<<(std::ostream& out, StrokeLineJoin linejoin);

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
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

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};


template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color fill_color) {
        fill_color_ = std::move(fill_color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color stroke_color) {
        stroke_color_ = std::move(stroke_color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_line_cap_ = line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_line_join_ = line_join;
        return AsOwner();
    }

    virtual void RenderAttrs(const RenderContext& context) const;
    
private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
};

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class Circle : public Object, public PathProps<Circle> {
public:
    Circle() : Object() {}
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

class Polyline : public Object, public PathProps<Polyline> {
public:
    Polyline() : Object() {}
    Polyline& AddPoint(Point point);

private:
    std::vector<Point> points_;

    void RenderObject(const RenderContext& context) const override;
};

class Text : public Object, public PathProps<Text> {
public:
    Text() : Object() {}
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);
private:
    Point position_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
    void RenderObject(const RenderContext& context) const override;
    std::string TransformText(std::string text);
};

class ObjectContainer {
public:
    template<typename Obj>
    void Add(Obj obj);

    virtual ~ObjectContainer() = default;
private:
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
};

template <typename Obj>
void ObjectContainer::Add(Obj obj) {
    AddPtr(std::make_unique<Obj>(std::move(obj)));
}

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};



class Document : public ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj);
    void AddPtr(std::unique_ptr<Object>&& obj);
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
    
};

template <typename Obj>
void Document::Add(Obj obj) {
    objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
}

//----------- PathProps --------------

template <typename Owner>
void PathProps<Owner>::RenderAttrs(const RenderContext& context) const {
    auto& out = context.out;
    if(fill_color_.has_value()) {
        out << " fill=\"" << fill_color_.value() << "\"";
    }
    if(stroke_color_.has_value()) {
        out << " stroke=\"" << stroke_color_.value() << "\"";   
    }
    if(stroke_width_.has_value()) {
        out << " stroke-width=\"" << stroke_width_.value() << "\"";  
    }
    if(stroke_line_cap_.has_value()) {
        out << " stroke-linecap=\"" << stroke_line_cap_.value() << "\"";
    }
    if(stroke_line_join_.has_value()) {
        out << " stroke-linejoin=\"" << stroke_line_join_.value() << "\"";
    }
}

} // namespace svg
