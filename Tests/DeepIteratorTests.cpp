#include "stdafx.h"
#include "CppUnitTest.h"

#include <DeepIterator.hpp>
#include <Algorithms.hpp>
#include <vector>
#include <numeric>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	struct TestNumber
	{
		TestNumber() {}
		TestNumber(int x) : number{x} {}
		int number;

		bool operator==(int other) { return number == other; }
		operator int() const { return number; }
	};

	using Nesting_0 = std::vector<TestNumber>;
	using Nesting_0_it = DeepIterator<typename Nesting_0::iterator>;
	using Nesting_1 = std::vector<std::vector<TestNumber>>;
	using Nesting_1_it = DeepIterator<typename Nesting_1::iterator>;
	using Nesting_2 = std::vector<std::vector<std::vector<TestNumber>>>;
	using Nesting_2_it = DeepIterator<typename Nesting_2::iterator>;

	static_assert(detail::DeepIteratorMaxLevel<typename Nesting_0::iterator>::value == 0,
		"DeepIteratorMaxLevel for level = 0 doesn't work");
	static_assert(detail::DeepIteratorMaxLevel<typename Nesting_1::iterator>::value == 1,
		"DeepIteratorMaxLevel for level = 1 doesn't work");
	static_assert(detail::DeepIteratorMaxLevel<typename Nesting_2::iterator>::value == 2,
		"DeepIteratorMaxLevel for level = 2 doesn't work");

	static_assert(Nesting_0_it::nestingLevel == 0, "");
	static_assert(std::is_same<Nesting_0_it::value_type, TestNumber>::value, "");
	static_assert(std::is_same<Nesting_0_it::reference, TestNumber&>::value, "");
	static_assert(std::is_same<Nesting_0_it::pointer, TestNumber*>::value, "");
	static_assert(std::is_same<Nesting_0_it::iterator_category, std::input_iterator_tag>::value, "");

	static_assert(Nesting_1_it::nestingLevel == 1, "");
	static_assert(std::is_same<Nesting_1_it::value_type, TestNumber>::value, "");
	static_assert(std::is_same<Nesting_1_it::reference, TestNumber&>::value, "");
	static_assert(std::is_same<Nesting_1_it::pointer, TestNumber*>::value, "");
	static_assert(std::is_same<Nesting_1_it::iterator_category, std::input_iterator_tag>::value, "");

	static_assert(Nesting_2_it::nestingLevel == 2, "");
	static_assert(std::is_same<Nesting_2_it::value_type, TestNumber>::value, "");
	static_assert(std::is_same<Nesting_2_it::reference, TestNumber&>::value, "");
	static_assert(std::is_same<Nesting_2_it::pointer, TestNumber*>::value, "");
	static_assert(std::is_same<Nesting_2_it::iterator_category, std::input_iterator_tag>::value, "");

	TEST_CLASS(DeepIteratorTests)
	{
	public:
		TEST_METHOD(nesting_0)
		{
			Nesting_0 x{ 0, 1, 2, 3 };

			auto it = Nesting_0_it{ x.begin(), x.end() };
			auto first = it;

			Assert::IsTrue(0 == *it);
			Assert::IsTrue(it == first);
			Assert::IsFalse(it != first);

			it++;

			Assert::IsTrue(1 == *it);
			Assert::IsFalse(it == first);
			Assert::IsTrue(it != first);

			++it;
			Assert::IsTrue(2 == *it);
			++it;
			Assert::IsTrue(3 == *it);

			Assert::IsTrue(it->number == 3);

			transformInPlace(deepBegin(x), deepEnd(x), [](TestNumber x)
			{
				return x + 1;
			});
			Assert::AreEqual(2, x[1].number);
			Assert::AreEqual(10, std::accumulate(deepBegin(x), deepEnd(x), 0));
		}

		TEST_METHOD(nesting_1)
		{
			Nesting_1 x{ Nesting_0{0, 1}, Nesting_0{2}, Nesting_0{3, 4} };

			auto it = Nesting_1_it{ x.begin(), x.end() };
			auto first = it;

			Assert::IsTrue(0 == *it);
			Assert::IsTrue(it == first);
			Assert::IsFalse(it != first);

			it++;

			Assert::IsTrue(1 == *it);
			Assert::IsFalse(it == first);
			Assert::IsTrue(it != first);

			++it;
			Assert::IsTrue(2 == *it);
			++it;
			Assert::IsTrue(3 == *it);

			Assert::IsTrue(it->number == 3);

			transformInPlace(deepBegin(x), deepEnd(x), [](TestNumber x) { return x + 1; });
			Assert::AreEqual(3, x[1][0].number);

			Assert::AreEqual(15, std::accumulate(deepBegin(x), deepEnd(x), 0));
		}

		TEST_METHOD(nesting_2)
		{
			Nesting_2 x
			{ 
				Nesting_1{Nesting_0{5, 4}},
				Nesting_1{Nesting_0{3}, Nesting_0{2}},
				Nesting_1{Nesting_0{1}},
			};

			auto it = Nesting_2_it{ x.begin(), x.end() };
			auto first = it;

			Assert::IsTrue(5 == *it);
			Assert::IsTrue(it == first);
			Assert::IsFalse(it != first);

			it++;

			Assert::IsTrue(4 == *it);
			Assert::IsFalse(it == first);
			Assert::IsTrue(it != first);

			++it;
			Assert::IsTrue(3 == *it);
			++it;
			Assert::IsTrue(2 == *it);

			Assert::IsTrue(it->number == 2);

			transformInPlace(deepBegin(x), deepEnd(x), [](TestNumber x)
			{
				return x + 1;
			});
			Assert::AreEqual(3, x[1][1][0].number);

			Assert::AreEqual(20, std::accumulate(deepBegin(x), deepEnd(x), 0));
		}
	};
}