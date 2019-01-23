#include "stdafx.h"
#include "CppUnitTest.h"

#include <KNN.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	TEST_CLASS(KnnTests)
	{
	public:
		using ListOfTracks = std::vector<FeatureVector<MFCC>>;

		AllGroups<MFCC> testGroups = AllGroups<MFCC>{
			FeatureGroup{
				"test_1",
				ListOfTracks{
					{{1.0, 2.0, 3.0}},
					{{3.0, 2.0, 4.0}},
					{{2.0, 3.0, 4.0}}
				}
			},
			FeatureGroup{
				"test_2",
				ListOfTracks{
					{{-1.0, 3.0, -3.0}},
					{{-3.0, 6.0, -3.0}},
					{{-1.0, 3.0, -1.0}}
				}
			},
		};

		TEST_METHOD(findKnn_k1_zeroCost)
		{
			int group = 0;
			int sample = 0;
			FeatureVector<MFCC> testSample = testGroups[group].features[sample];

			auto result = findKnn(1, testSample, testGroups, CostSquaredDistance{});

			Assert::AreEqual(1, int(result.size()));
			Assert::AreEqual(group, result[0].sample);
			Assert::AreEqual(sample, result[0].group);
			Assert::AreEqual(0.0, result[0].cost);
			Assert::AreEqual(group, classifyKnn(result));

			group = 1;
			sample = 1;
			testSample = testGroups[group].features[sample];

			result = findKnn(1, testSample, testGroups, CostSquaredDistance{});

			Assert::AreEqual(1, int(result.size()));
			Assert::AreEqual(group, result[0].sample);
			Assert::AreEqual(sample, result[0].group);
			Assert::AreEqual(0.0, result[0].cost);
			Assert::AreEqual(group, classifyKnn(result));
		}

		TEST_METHOD(findKnn_k3)
		{
			FeatureVector<MFCC> testSample = { { 2.0, 3.0, 2.0 } };

			auto getCost = CostSquaredDistance{};
			auto result = findKnn(3, testSample, testGroups, getCost);

			Assert::AreEqual(3, int(result.size()));
			Assert::AreEqual(0, result[0].sample);
			Assert::AreEqual(0, result[0].group);
			Assert::AreEqual(3.0, result[0].cost);
			Assert::AreEqual(0, result[1].group);
			Assert::AreEqual(2, result[1].sample);
			Assert::AreEqual(0, result[2].group);
			Assert::AreEqual(1, result[2].sample);
			Assert::AreEqual(0, classifyKnn(result));

			testSample = { { 0.0, 2.0, 0.0 } };

			result = findKnn(3, testSample, testGroups, getCost);

			Assert::AreEqual(3, int(result.size()));
			Assert::AreEqual(2, result[0].sample);
			Assert::AreEqual(1, result[0].group);
			Assert::AreEqual(3.0, result[0].cost);
			Assert::AreEqual(0, result[1].group);
			Assert::AreEqual(0, result[1].sample);
			Assert::AreEqual(1, result[2].group);
			Assert::AreEqual(0, result[2].sample);
			Assert::AreEqual(1, classifyKnn(result));
		}
	};
}