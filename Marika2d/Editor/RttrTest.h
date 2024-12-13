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

class TestBase
{
    friend void RttrTestFunc();
public:
    int field1 = 123;
    float field2 = 123.456f;
    std::string field3 = "Hello World";
    std::vector<std::shared_ptr<TestBase>> field4;

    MRK_REFL_BASE()
    MRK_REFL_TYPE(TestBase)
    MRK_REFL_PROP("field1", &TestBase::field1)
    MRK_REFL_PROP("field2", &TestBase::field2)
    MRK_REFL_PROP("field3", &TestBase::field3)
    MRK_REFL_PROP("field4", &TestBase::field4)
    MRK_REFL_REND
};

class TestDerived1 : public TestBase
{
public:
    double field5 = 123.456789;

    MRK_REFL_BASE(TestBase)
    MRK_REFL_TYPE(TestDerived1)
    MRK_REFL_PROP("field5", &TestDerived1::field5)
    MRK_REFL_REND
};

class TestDerived2 : public TestBase
{
public:
    short field5 = 123;

    MRK_REFL_BASE(TestBase)
    MRK_REFL_TYPE(TestDerived2)
    MRK_REFL_PROP("field5", &TestDerived2::field5)
    MRK_REFL_REND
};

inline void RttrTestFunc()
{
    circle c_1("Circle #1");
    shape& my_shape = c_1;

    c_1.set_visible(true);
    c_1.points = std::vector<point2d>(2, point2d(1, 1));
    c_1.points[1].x = 23;
    c_1.points[1].y = 42;

    c_1.position.x = 12;
    c_1.position.y = 66;

    c_1.radius = 5.123;
    c_1.color_ = color::red;

    // additional braces are needed for a VS 2013 bug
    c_1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };

    c_1.no_serialize = 12345;

    MrkNew::ReflectSystem::ToJson(my_shape);


    TestBase base;
    base.field4.emplace_back(new TestDerived1());
    base.field4.emplace_back(new TestDerived2());

    MrkNew::ReflectSystem::ToJson(base);
}