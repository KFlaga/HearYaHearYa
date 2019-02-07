#include "stdafx.h"
#include "CppUnitTest.h"

#include <KMeans.hpp>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	TEST_CLASS(KMeansTests)
	{
	public:
		template<typename Func>
		void expectThrow(Func f)
		{
			try
			{
				f();
			}
			catch (const std::exception& e)
			{
				return;
			}
			Assert::Fail(L"Throw expected");
		}

		TEST_METHOD(test_findKMeans_idealMatch)
		{
			std::vector<MFCC> input
			{
				MFCC{1.0, 2.0, 3.0},
				MFCC{2.0, 5.0, 7.0},
				MFCC{1.0, 3.0, -2.0}
			};

			auto result = findKMeans(3, input);
			Assert::IsTrue(input == result);
		}

		TEST_METHOD(test_findKMeans_k1)
		{
			std::vector<MFCC> input
			{
				MFCC{1.0, 5.0, 4.0},
				MFCC{4.0, 2.0, 7.0},
				MFCC{1.0, 2.0, -2.0}
			};

			auto result = findKMeans(1, input);
			Assert::IsTrue(result == std::vector<MFCC>{ MFCC{2.0, 3.0, 3.0} });
		}

		TEST_METHOD(test_findKMeans_twoClusters)
		{
			std::vector<MFCC> input
			{
				MFCC{3.0, 5.0, -2.0}, // C2
				MFCC{-2.5, -3.0, 4.0}, // C1
				MFCC{0.5, -1.0, 6.0}, // C1
				MFCC{2.0, 4.0, 1.0}, // C2
				MFCC{1.0, 0.0, -3.5}, // C2
			};

			auto initialMeans = [](std::size_t, const std::vector<MFCC>&)
			{
				return std::vector<MFCC>
				{
					MFCC{ -0.5, 0.0, 0.0 },
					MFCC{ 1.0, 2.0, 0.0 },
				};
			};

			auto expected = std::vector<MFCC>
			{
				MFCC{ -1.0, -2.0, 5.0 },
				MFCC{ 2.0, 3.0, -1.5 },
			};

			auto result = findKMeans(2, input, initialMeans);
			Assert::IsTrue(result == expected);
		}
	};
}
