#include "RttrTest.h"
#include "Third/rttr/registration.h"



//static void rttr_auto_register_reflection_function_(); 
//namespace 
//{
//    struct rttr__auto__register__ 
//    {
//        rttr__auto__register__() 
//        {
//            rttr_auto_register_reflection_function_();
//        }
//    };
//}
//static const rttr__auto__register__ auto_register__6; 
//static void rttr_auto_register_reflection_function_()
//{
//    rttr::registration::class_<shape>("shape")
//        .property("visible", &shape::get_visible, &shape::set_visible)
//        .property("color", &shape::color_)
//        .property("name", &shape::name)
//        .property("position", &shape::position)
//        .property("dictionary", &shape::dictionary)
//    ;
//
//    rttr::registration::class_<circle>("circle")
//        .property("radius", &circle::radius)
//        .property("points", &circle::points)
//        .property("no_serialize", &circle::no_serialize)
//        (
//            rttr::metadata("NO_SERIALIZE", true)
//        )
//        ;
//
//    rttr::registration::class_<point2d>("point2d")
//        .constructor()(rttr::policy::ctor::as_object)
//        .property("x", &point2d::x)
//        .property("y", &point2d::y)
//        ;
//
//
//    rttr::registration::enumeration<color>("color")
//        (
//            rttr::value("red", color::red),
//            rttr::value("blue", color::blue),
//            rttr::value("green", color::green)
//        );
//}
