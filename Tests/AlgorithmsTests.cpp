#include "stdafx.h"
#include "CppUnitTest.h"

#include <Algorithms.hpp>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	TEST_CLASS(AlgorithmsTests)
	{
	public:
		TEST_METHOD(Accumulate2)
		{
			std::vector<double> a{ 0.0, 1.0, 2.0, 3.0 };
			std::vector<double> b{ 3.0, 5.0, 1.0, 2.0 };
			{
				double result = accumulate2(a, b, 2.0, [](double x, double y, double z)
				{
					return z + (x - y);
				});
				Assert::AreEqual(-3.0, result);
			}
			{
				double result = accumulate2(std::begin(a), std::end(a), std::begin(b), 3.0, [](double x, double y, double z)
				{
					return z + (x + y);
				});
				Assert::AreEqual(20.0, result);
			}
		}

		TEST_METHOD(ForEachWithEach)
		{
			std::vector<double> a{ 0.0, 1.0, 2.0, 3.0 };
			{
				double result = 0.0;
				forEachWithEach(std::begin(a), std::end(a), IncludeSelf{}, [&](double x, double y)
				{
					result += (x + y);
				});
				Assert::AreEqual(30.0, result);
			}
			{
				double result = 0.0;
				forEachWithEach(std::begin(a), std::end(a), ExcludeSelf{}, [&](double x, double y)
				{
					result += (x + y);
				});
				Assert::AreEqual(18.0, result);
			}
		}

		TEST_METHOD(TransformInPlace)
		{
			std::vector<double> a{ 0.0, 1.0, 2.0, 3.0 };
			{
				transformInPlace(std::begin(a), std::end(a), [](double x)
				{
					return x * 2.0;
				});
				Assert::IsTrue(a == std::vector<double>{0.0, 2.0, 4.0, 6.0});
			}
			{
				transformInPlace(a, [](double x)
				{
					return x * 3.0;
				});
				Assert::IsTrue(a == std::vector<double>{0.0, 6.0, 12.0, 18.0});
			}
		}

		TEST_METHOD(TransformFirstInPlace)
		{
			std::vector<double> a{ 0.0, 1.0, 2.0 };
			std::vector<double> b{ 1.0, 2.0, 3.0  };
			{
				transformFirstInPlace(std::begin(a), std::end(a), std::begin(b), [](double x, double y)
				{
					return x + y;
				});
				Assert::IsTrue(a == std::vector<double>{1.0, 3.0, 5.0});
			}
			{
				transformFirstInPlace(a, b, [](double x, double y)
				{
					return x * 2.0 + y * 3.0;
				});
				Assert::IsTrue(a == std::vector<double>{5.0, 12.0, 19.0});
			}
		}

		TEST_METHOD(AccumulateNested)
		{
			std::vector<std::vector<double>> a{ { 1.0, 2.0 }, { 3.0, 4.0 } };
			{
				double result = accumulateNested(std::begin(a), std::end(a), 3.0, [](double x, double acc)
				{
					return acc + x;
				});
				Assert::AreEqual(13.0, result);
			}
		}
	};
}
