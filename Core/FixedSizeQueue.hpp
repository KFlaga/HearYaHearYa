#pragma once

#include <vector>
#include <algorithm>

namespace eave
{
	// Sorted list with upper limit elements
	// so that it holds at most N biggests elements according to "Comparer"
	template<typename T, typename Comparer_ = std::less<T>>
	class FixedSizeQueue
	{
	public:
		using ValueType = T;
		using Comparer = Comparer_;

		FixedSizeQueue(std::size_t maxSize_, Comparer compare_ = Comparer{}) :
			maxCount{ maxSize_ },
			compare{ compare_ }
		{
			data.reserve(maxCount + 1);
		}

		void push(const ValueType& item)
		{
			// Find last element which is bigger than "item"
			// Start from end as in long run most of elements are expected to be smaller than few best ones
			auto biggerOne = std::find_if(data.rbegin(), data.rend(), [&](ValueType& x) {
				return compare(item, x);
			});

			if (biggerOne == data.rbegin() && size() > maxSize())
			{
				// New element is smaller than all others and we reached max capacity - ignore it
				return;
			}

			// We want to insert current item after last bigger one
			// As reverse iterators doesn't cooperate with "insert" we need to change it to normal one
			auto pos = std::distance(data.rbegin(), biggerOne);
			data.insert(data.end() - pos, item);

			if (size() > maxSize())
			{
				data.pop_back();
			}
		}

		const ValueType& operator[](int idx) const
		{
			return data[idx];
		}

		ValueType& operator[](int idx)
		{
			return data[idx];
		}

		std::size_t size() const
		{
			return data.size();
		}

		std::size_t maxSize() const
		{
			return maxCount;
		}

		auto begin()
		{
			return data.begin();
		}

		auto begin() const
		{
			return data.begin();
		}

		auto end()
		{
			return data.begin();
		}

		auto end() const
		{
			return data.begin();
		}

		void clear()
		{
			data.clear();
		}

	private:
		std::vector<ValueType> data;
		Comparer compare;
		std::size_t maxCount;
	};
}
