#pragma once

#include <type_traits>

#ifndef MRK_HAS_MEMFUNC
#define MRK_TRAIT_HAS_MEMFUNC(func)	\
template <typename T, typename = std::void_t<>> struct TraitHas##func : std::false_type{};	\
template <typename T> struct TraitHas##func<T, std::void_t<decltype(std::declval<T>().func())>> : std::true_type {};
#endif // !MRK_HAS_MEMFUNC
