#pragma once

#include <map>
#include <vector>
#include <list>
#include <unordered_map>
#include <set>
#include <memory>
#include <optional>
#include <iostream>

namespace Mrk
{
    template <typename T>
    struct has_output_operator {
        template <typename U> static auto test(U* ptr) -> decltype(std::cout << *ptr, std::true_type());
        template <typename U> static std::false_type test(...);
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };

    template <typename T>
    struct has_input_operator {
        template <typename U> static auto test(U* ptr) -> decltype(std::cin >> *ptr, std::true_type());
        template <typename U> static std::false_type test(...);
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };

    template<auto Func, typename T>
    struct is_callable
    {
        template<typename U> static auto test(U* p) -> decltype(Func(*p), std::true_type());
        static std::false_type test(...);
        static constexpr bool value = decltype(test(static_cast<T*>(nullptr)))::value;
    };

	template<typename T>
	struct is_seq_container {
		static constexpr bool value = false;
	};

	template<typename T, typename Alloc>
	struct is_seq_container<std::vector<T, Alloc>> {
		static constexpr bool value = true;
	};

	template<typename T, typename Alloc>
	struct is_seq_container<std::list<T, Alloc>> {
		static constexpr bool value = true;
	};

	template<typename T, size_t N>
	struct is_seq_container<std::array<T, N>> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct is_ass_container {
		static constexpr bool value = false;
	};

	template<typename Key, typename T, typename Compare, typename Alloc>
	struct is_ass_container<std::map<Key, T, Compare, Alloc>> {
		static constexpr bool value = true;
	};

	template<typename Key, typename T, typename Compare, typename Alloc>
	struct is_ass_container<std::multimap<Key, T, Compare, Alloc>> {
		static constexpr bool value = true;
	};

	template<typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
	struct is_ass_container<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> {
		static constexpr bool value = true;
	};

	template<typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
	struct is_ass_container<std::unordered_multimap<Key, T, Hash, KeyEqual, Alloc>> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct is_unique_ptr {
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_unique_ptr<std::unique_ptr<T>> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct is_shared_ptr {
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_shared_ptr<std::shared_ptr<T>> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct is_weak_ptr {
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_weak_ptr<std::weak_ptr<T>> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct is_optional {
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_optional<std::optional<T>> {
		static constexpr bool value = true;
	};

	template<typename T>
	struct type_extract;

	template<typename _class, typename _field>
	struct type_extract<_field _class::*>
	{
		using class_t = _class;
		using field_t = _field;
	};

	template<typename _ret, typename ...Args>
	struct  type_extract<_ret(*)(Args...)>
	{
		using ret_t = _ret;
	};

	template<typename _class, typename _ret, typename Arg>
	struct  type_extract<_ret(_class::*)(Arg)>
	{
		using class_t = _class;
		using ret_t = _ret;
		using arg_1_t = Arg;
	};

	template<typename _class, typename _ret, typename ...Args>
	struct  type_extract<_ret(_class::*)(Args...)>
	{
		using class_t = _class;
		using ret_t = _ret;
	};

	template<typename Dest, typename Source>
	inline Dest ForceCast(Source source)
	{
		union
		{
			Source source;
			Dest dest;
		} castunion;
		castunion.source = source;
		return castunion.dest;
	}

	template<typename Dst, typename Src>
	inline void ForceTamper(Dst& dst, const Src& src, size_t offset)
	{
		Src* tamper = reinterpret_cast<Src*>((char*)&dst + offset);
		std::memcpy(tamper, &src, sizeof(Src));
	}

	template<typename T, typename Dst>
	inline T& ForceExtract(const Dst& dst, size_t offset)
	{
		T* t = reinterpret_cast<T*>((char*)&dst + offset);
		return *t;
	}

	template<typename _class, typename _field> constexpr
		size_t OffsetExtract(_field _class::* field)
	{
		return (char*)&((_class*)nullptr->*field) - (char*)nullptr;
	}
}

