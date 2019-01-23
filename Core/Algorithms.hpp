#pragma once

#include <algorithm>

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
}
