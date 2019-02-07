#pragma once

#include "TypeTraits.hpp"
#include "Expect.hpp"
#include <algorithm>
#include <iterator>

namespace eave
{
	namespace detail
	{
		template<typename Iterator, std::size_t level, typename IsNextLevelIterable>
		struct DeepIteratorMaxLevelImpl { };

		template<typename Iterator, std::size_t level>
		struct DeepIteratorMaxLevelImpl<Iterator, level, std::false_type>
		{
			// value_type of Iterator is not an iterable - we reached max nesting level
			using value_type = typename Iterator::value_type;
			static constexpr std::size_t value = level;
		};

		template<typename Iterator, std::size_t level>
		struct DeepIteratorMaxLevelImpl<Iterator, level, std::true_type>
		{
			// value_type of Iterator is an iterable - so lets extract this iterable
			using iterable_type = typename Iterator::value_type;
			using next_level_value_type = typename iterable_type::iterator::value_type;
			// Move to next level - depending on whether on next level is an iterable as well proper specialization will be used
			static constexpr std::size_t value = DeepIteratorMaxLevelImpl<
				typename iterable_type::iterator, level + 1, traits::is_iterable_t<next_level_value_type>>::value;
		};

		template<typename Iterator>
		struct DeepIteratorMaxLevel
		{
			using value_type = typename Iterator::value_type;
			static constexpr std::size_t value = DeepIteratorMaxLevelImpl<Iterator, 0, traits::is_iterable_t<value_type>>::value;
		};

		template<typename Iterator>
		constexpr bool isConstIterator = std::is_const_v<std::remove_reference_t<Iterator::reference>>;

		template<typename Iterator>
		using DeepIteratorNextLevelIterator = std::conditional_t<
			isConstIterator<Iterator>,
			typename Iterator::value_type::const_iterator,
			typename Iterator::value_type::iterator>;
	}

	// todo: description
	// "maxLevel" is required to be not greater than actual nesting level, but may be smaller
	// It seems iterator and value_type needs to be default constructible
	// TODO: its about 2 times slower to use deep iterator compared to "nested" algorithm for simple functions
	template<typename Iterator, std::size_t maxLevel = detail::DeepIteratorMaxLevel<Iterator>::value>
	class DeepIterator : protected DeepIterator<detail::DeepIteratorNextLevelIterator<Iterator>, maxLevel - 1>
	{
		using next_level = DeepIterator<detail::DeepIteratorNextLevelIterator<Iterator>, maxLevel - 1>;
	public:
		static constexpr std::size_t nestingLevel = maxLevel;

		using self_type = DeepIterator<Iterator, maxLevel>;
		using value_type = typename next_level::value_type;
		using reference = typename next_level::reference;
		using pointer = typename next_level::pointer;
		using iterator_category = typename next_level::iterator_category; // TODO
		using difference_type = typename next_level::difference_type;

		explicit DeepIterator() {}

		explicit DeepIterator(Iterator begin_, Iterator end_ = Iterator{})
		{
			setRange(begin_, end_);
		}

		self_type operator++()
		{
			next();
			return *this;
		}

		self_type operator++(int)
		{
			self_type tmp = *this;
			next();
			return tmp;
		}

		reference operator*()
		{
			return dereferenceLastLevel();
		}

		pointer operator->()
		{
			return getPointer();
		}

		bool operator==(const self_type& other) const
		{
			return equals(other);
		}

		bool operator!=(const self_type& other) const
		{
			return !(*this == other);
		}

	protected:

		reference dereferenceLastLevel()
		{
			return next_level::dereferenceLastLevel();
		}

		bool isEnd() const
		{
			return current == end;
		}

		void next()
		{
			next_level::next();
			if (next_level::isEnd())
			{
				++current;
				if (!isEnd())
				{
					next_level::setRange(current->begin(), current->end());
				}
			}
		}

		void setRange(Iterator begin_, Iterator end_)
		{
			current = begin_;
			end = end_;
			if (!isEnd())
			{
				next_level::setRange(current->begin(), current->end());
			}
		}

		bool equals(const self_type& other) const
		{
			return current == other.current &&
				   (isEnd() || next_level::equals(other));
		}

		pointer getPointer()
		{
			return next_level::getPointer();
		}

	private:
		Iterator current;
		Iterator end;
	};

	template<typename Iterator>
	class DeepIterator<Iterator, 0>
	{
	public:
		static constexpr std::size_t nestingLevel = 0;

		using self_type = DeepIterator<Iterator, 0>;
		using value_type = typename Iterator::value_type;
		using reference = typename Iterator::reference;
		using pointer = typename Iterator::pointer;

		using iterator_category = std::input_iterator_tag; // TODO
		using difference_type = typename Iterator::difference_type;

		explicit DeepIterator() {}

		explicit DeepIterator(Iterator begin_, Iterator end_) :
			current{ begin_ }, end{ end_ }
		{

		}

		self_type operator++()
		{
			next();
			return *this;
		}

		self_type operator++(int)
		{
			next();
			return *this;
		}

		reference operator*()
		{
			return *current;
		}

		pointer operator->()
		{
			return getPointer();
		}

		bool operator==(const self_type& other) const
		{
			return other.current == current;
		}

		bool operator!=(const self_type& other) const
		{
			return !(*this == other);
		}

	protected:
		reference dereferenceLastLevel()
		{
			return *current;
		}

		bool isEnd() const
		{
			return current == end;
		}

		void next()
		{
			++current;
		}

		void setRange(Iterator begin_, Iterator end_)
		{
			current = begin_;
			end = end_;
		}

		bool equals(const self_type& other) const
		{
			return current == other.current;
		}

		pointer getPointer()
		{
			return current.operator->();
		}

	private:
		Iterator current;
		Iterator end;
	};

	template<typename Container>
	auto deepBegin(Container&& container)
	{
		using iterator = decltype(container.begin());
		return DeepIterator<iterator>{container.begin(), container.end()};
	}

	template<typename Container>
	auto deepEnd(Container&& container)
	{
		using iterator = decltype(container.end());
		return DeepIterator<iterator>{container.end(), container.end()};
	}
}
