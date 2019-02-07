#pragma once

#include <type_traits>

namespace eave
{
	namespace traits
	{
		// Use SFINAE in decltype + "..." as worst fit trick to determine if T has typedefed "iterator"
		// It surely doesn't work in all scenarios as it requires both container and iterator to be default constructible
		template <typename T>
		constexpr auto is_iterable(const T&) -> decltype(typename T::iterator{}, std::true_type{})
		{
			return std::true_type{};
		}

		constexpr auto is_iterable(...)
		{
			return std::false_type{};
		}

		template<typename T>
		using is_iterable_t = decltype(is_iterable(T{}));

		template<typename T>
		constexpr bool is_iterable_v = is_iterable_t<T>::value;

		using IsContainer = std::true_type;
		using IsValue = std::false_type;
	}
}
