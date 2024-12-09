#pragma once

#include "Third/rttr/registration.h"
#include "Third/rttr/type.h"

enum class color
{
    red,
    green,
    blue
};

struct point2d
{
    point2d() {}
    point2d(int x_, int y_) : x(x_), y(y_) {}
    int x = 0;
    int y = 0;
};

struct shape
{
    shape(std::string n) : name(n) {}

    void set_visible(bool v) { visible = v; }
    bool get_visible() const { return visible; }

    color color_ = color::blue;
    std::string name = "";
    point2d position;
    std::map<color, point2d> dictionary;

    RTTR_ENABLE()
private:
    bool visible = false;
};

struct circle : shape
{
    circle(std::string n) : shape(n) {}

    double radius = 5.2;
    std::vector<point2d> points;

    int no_serialize = 100;

    static inline bool re = []()
        {
            rttr::registration::class_<shape>("shape")
                .property("visible", &shape::get_visible, &shape::set_visible)
                .property("color", &shape::color_)
                .property("name", &shape::name)
                .property("position", &shape::position)
                .property("dictionary", &shape::dictionary)
                ;

            rttr::registration::class_<circle>("circle")
                .property("radius", &circle::radius)
                .property("points", &circle::points)
                .property("no_serialize", &circle::no_serialize)
                (
                    rttr::metadata("NO_SERIALIZE", true)
                    )
                ;

            rttr::registration::class_<point2d>("point2d")
                .constructor()(rttr::policy::ctor::as_object)
                .property("x", &point2d::x)
                .property("y", &point2d::y)
                ;


            rttr::registration::enumeration<color>("color")
                (
                    rttr::value("red", color::red),
                    rttr::value("blue", color::blue),
                    rttr::value("green", color::green)
                    );

            return true;
        }();
    RTTR_ENABLE(shape)
};