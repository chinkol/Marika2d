#pragma once

#include "Third/rttr/registration.h"
#include "Third/rttr/type.h"

#include "Core/Reflect/Reflect.h"

#include <iostream>
#include <map>


class TestBase
{
    friend void RttrTestFunc();
public:
    int field1 = 123;
    float field2 = 123.456f;
    std::string field3 = "Hello World";
    std::vector<TestBase*> field4;
    std::map<std::string, TestBase*> fieldtest;

    MRK_RTTR_BASE()
};

MRK_RTTR_TYPE(TestBase)
(
    rttr::registration::class_<TestBase>("TestBase")
    .constructor<>()
    .property("field1", &TestBase::field1)
    .property("field2", &TestBase::field2)
    .property("field3", &TestBase::field3)
    .property("field4", &TestBase::field4)
    .property("fieldtest", &TestBase::fieldtest);
)

class TestDerived1 : public TestBase
{
public:
    double field5 = 123.456789;

    MRK_RTTR_BASE(TestBase)
};

MRK_RTTR_TYPE(TestDerived1)
(
    rttr::registration::class_<TestDerived1>("TestDerived1")
    .constructor<>()
    .property("field5", &TestDerived1::field5);
)

class TestDerived2 : public TestBase
{
public:
    short field5 = 321;

    MRK_RTTR_BASE(TestBase)
};

MRK_RTTR_TYPE(TestDerived2)
(
    rttr::registration::class_<TestDerived2>("TestDerived2")
    .constructor<>()
    .property("field5", &TestDerived2::field5);
)

class TestDerived3 : public TestBase
{
public:
    std::string field5 = "World Hello";

    MRK_RTTR_BASE(TestBase) 
};

MRK_RTTR_TYPE(TestDerived3)
(
    rttr::registration::class_<TestDerived3>("TestDerived3")
    .constructor<>()
    .property("field5", &TestDerived3::field5);
)

static inline bool k = true;

inline void RttrTestFunc()
{ 
    TestBase base;
    base.field4.emplace_back(new TestDerived1());
    base.field4.emplace_back(new TestDerived2());
    base.field4.emplace_back(new TestDerived3());
    base.fieldtest.emplace("1", new TestDerived1());
    base.fieldtest.emplace("2", new TestDerived2());
    base.fieldtest.emplace("3", new TestDerived3());
    base.fieldtest.emplace("4", new TestBase());

    base.field1 = 0;
    base.field2 = 0;
    base.field3 = "NULL";
    base.field4.clear();
    base.fieldtest.clear();

    auto n = rttr::type::get_by_name("TestBase").get_constructor().invoke().get_type().get_name();

    auto json = MrkNew::ReflectSystem::ToJson(base);
    auto rttrobj = MrkNew::ReflectSystem::FromJson(json);

    std::shared_ptr<TestBase> test;
    if (rttrobj.convert<decltype(test)>(test))
    {
        std::cout << "Yes" << std::endl;
    }

    std::cout << MrkNew::ReflectSystem::ToJson(base);
}  