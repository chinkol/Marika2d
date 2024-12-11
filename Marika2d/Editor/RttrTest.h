#pragma once

#include "Third/rttr/registration.h"
#include "Third/rttr/type.h"

#include "Core/Reflect/Reflect.h"

#include <iostream>

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

    void hello() { std::cout << "hello world"; }
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

    MRK_REFL_BASE(shape)
    MRK_REFL_TYPE(circle)
    MRK_REFL_FUNC("hello", &shape::hello)
    MRK_REFL_PROP("visible", &shape::get_visible, &shape::set_visible)
    MRK_REFL_PROP("name", &shape::name)
    MRK_REFL_META("flag1", true)
    MRK_REFL_META("flag2", false)
    MRK_REFL_REND
};