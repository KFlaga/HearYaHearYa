#pragma once

#include <algorithm>
#include <type_traits>
#include <numeric>

namespace eave
{
	// Iterate over two sequences and applies funcion for each pair of elements with accumulator
	template<typename T, typename U, template<typename...> class Container, typename Func>
	T accumulate2(const Container<U>& a, const Container<U>& b, T initial, Func f)
	{
		for (std::size_t i = 0; i < a.size(); ++i)
		{
			initial = f(a[i], b[i], initial);
		}
		return initial;
	}

	// Iterate over two sequences and applies funcion for each pair of elements with accumulator
	template<typename InputIt1, typename InputIt2, typename T, typename Func>
	T accumulate2(InputIt1 begin1, InputIt1 end1, InputIt2 begin2, T initial, Func f)
	{
		for (; begin1 != end1; begin1++, begin2++)
		{
			initial = f(*begin1, *begin2, initial);
		}
		return initial;
	}

	struct ExcludeSelf {};
	struct IncludeSelf {};

	// Visits: (a1, a1), (a1, a2) ... (a1, an), (a2, a2), (a2, a3) ... (a2, an), (a3, a3), (a3, a4) ... (an-1, an), (an, an)
	template<typename InputIt, typename Func>
	void forEachWithEach(InputIt begin1, InputIt end1, IncludeSelf, Func f)
	{
		for (auto it1 = begin1; it1 != end1; ++it1)
		{
			for (auto it2 = it1; it2 != end1; ++it2)
			{
				f(*it1, *it2);
			}
		}
	}
	// Visits: (a1, a2), (a1, a3) ... (a1, an), (a2, a3), (a2, a4) ... (a2, an), (a3, a4), (a3, a5) ... (an-1, an)
	template<typename InputIt, typename Func>
	void forEachWithEach(InputIt begin1, InputIt end1, ExcludeSelf, Func f)
	{
		for (auto it1 = begin1; it1 != end1; ++it1)
		{
			for (auto it2 = it1 + 1; it2 != end1; ++it2)
			{
				f(*it1, *it2);
			}
		}
	}

	// Transforms each element in sequence in-place
	template<typename InputOutputIt, typename Func>
	void transformInPlace(InputOutputIt begin1, InputOutputIt end1, Func f)
	{
		for (; begin1 != end1; begin1++)
		{
			*begin1 = f(*begin1);
		}
	}

	// Transforms each element in sequence in-place
	template<typename T, template<typename...> class Container, typename Func>
	void transformInPlace(Container<T>& a, Func f)
	{
		transformInPlace(a.begin(), a.end(), f);
	}

	// Transforms each element in first sequence in-place
	template<typename InputOutputIt, typename InputIt, typename Func>
	void transformFirstInPlace(InputOutputIt begin1, InputOutputIt end1, InputIt begin2, Func f)
	{
		for (; begin1 != end1; begin1++, begin2++)
		{
			*begin1 = f(*begin1, *begin2);
		}
	}

	// Transforms each element in first sequence in-place
	template<typename T, typename U, template<typename...> class Container, typename Func>
	void transformFirstInPlace(Container<T>& a, const Container<U>& b, Func f)
	{
		transformFirstInPlace(a.begin(), a.end(), b.begin(), f);
	}

	template<typename T>
	struct divide_by
	{
		T operator()(T a) const
		{
			return a / d;
		}

		T operator()(T a, T b) const
		{
			return a / b;
		}

		T d;
	};

	template<typename InputIt, typename T, typename Func>
	T accumulateNested(InputIt begin1, InputIt end1, T initial, Func f);

	namespace detail
	{
		template<typename>
		struct IsContainer_t : std::false_type {};

		template<typename T, template <typename...> class Container, typename... Args>
		struct IsContainer_t<Container<T, Args...>> : std::true_type {};

		using IsContainer = std::true_type;
		using IsValue = std::false_type;

		template<typename InputIt, typename T, typename Func>
		T accumulateNestedImpl(InputIt begin1, InputIt end1, T initial, Func f, IsValue)
		{
			return std::accumulate(begin1, end1, initial, f);
		}

		template<typename InputIt, typename T, typename Func>
		T accumulateNestedImpl(InputIt begin1, InputIt end1, T initial, Func f, IsContainer)
		{
			for (; begin1 != end1; begin1++)
			{
				initial = accumulateNested(std::begin(*begin1), std::end(*begin1), initial, f);
			}
			return initial;
		}
	}

	// Iterates over each element in nested sequence and applies funcion for each pair of elements with accumulator
	template<typename InputIt, typename T, typename Func>
	T accumulateNested(InputIt begin1, InputIt end1, T initial, Func f)
	{
		using ContainerOrValue = std::remove_const_t<std::remove_reference_t<decltype(*begin1)>>;
		return detail::accumulateNestedImpl(begin1, end1, initial, f, detail::IsContainer_t<ContainerOrValue>{});
	}

	// Finds element with smallest cost computed by given cost function
	template<typename InputIt, typename CostFunc>
	InputIt findMinCostElement(InputIt begin1, InputIt end1, CostFunc getCost)
	{
		if (begin1 == end1) { return begin1; }

		auto minElement = begin1;
		double minCost = getCost(*begin1);

		for (begin1++; begin1 != end1; begin1++)
		{
			double cost = getCost(*begin1);
			if (cost < minCost)
			{
				minElement = begin1;
				minCost = cost;
			}
		}
		return minElement;
	}
}
