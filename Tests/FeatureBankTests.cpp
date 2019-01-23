#include "stdafx.h"
#include "CppUnitTest.h"

#include <Features.hpp>
#include <sstream>
#include <cstdlib>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	TEST_CLASS(FeaturesTests)
	{
	public:
		int featureVectorSize = 2;

		template<typename T>
		struct OfType {};

		double createFeatures(OfType<double>)
		{
			return 0.0;
		}

		template<typename T>
		std::vector<T> createFeatures(OfType<std::vector<T>>)
		{
			std::vector<T> x;
			for (int i = 0; i < featureVectorSize; ++i)
			{
				x.push_back(createFeatures(OfType<T>{}));
			}
			return x;
		}

		FeatureBank prepareTestFetures()
		{
			FeatureBank bank;

			FeatureGroup group_1;
			group_1.command = "aaa";
			group_1.features = createFeatures(OfType<std::vector<FeatureVector<MFCC>>>{});

			FeatureGroup group_2;
			group_2.command = "bbb";
			group_2.features = createFeatures(OfType<std::vector<FeatureVector<MFCC>>>{});

			bank.allGroups.push_back(group_1);
			bank.allGroups.push_back(group_2);

			return bank;
		}

		TEST_METHOD(save)
		{
			FeatureBank bank = prepareTestFetures();
			std::stringstream output;

			bank.save(output);

			std::string result = output.str();
			std::string expected = "[ { \"command\" : \"aaa\", \"features\" : [ [ [ 0, 0 ], [ 0, 0 ] ], [ [ 0, 0 ], [ 0, 0 ] ] ] }, ";
			expected += "{ \"command\" : \"bbb\", \"features\" : [ [ [ 0, 0 ], [ 0, 0 ] ], [ [ 0, 0 ], [ 0, 0 ] ] ] } ]";

			Assert::AreEqual(expected, result);
		}

		TEST_METHOD(load_empty)
		{
			FeatureBank bank;

			std::stringstream input;

			input << "[ ]";

			bank.load(input);

			Assert::IsTrue(bank.allGroups.size() == 0);
		}

		TEST_METHOD(load_oneEmptyGroup)
		{
			FeatureBank bank;

			std::stringstream input;

			input << "[ { \"command\" : \"test\", \"features\" : [ ] } ]";

			bank.load(input);

			Assert::IsTrue(bank.allGroups.size() == 1);
			Assert::IsTrue(bank.allGroups[0].command == "test");
			Assert::IsTrue(bank.allGroups[0].features.size() == 0);
		}

		TEST_METHOD(load_twoEmptyGroups)
		{
			FeatureBank bank;

			std::stringstream input;

			input << "[ { \"command\" : \"test1\", \"features\" : [ ] }, { \"command\" : \"test2\", \"features\" : [ ] } ]";

			bank.load(input);

			Assert::IsTrue(bank.allGroups.size() == 2);
			Assert::IsTrue(bank.allGroups[0].command == "test1");
			Assert::IsTrue(bank.allGroups[0].features.size() == 0);
			Assert::IsTrue(bank.allGroups[1].command == "test2");
			Assert::IsTrue(bank.allGroups[1].features.size() == 0);
		}

		TEST_METHOD(load_oneFullGroup)
		{
			FeatureBank bank;

			std::stringstream input;

			input << "[{ \"command\" : \"test\", \"features\" : [ [ [ 0, 0 ], [ 0, 0 ] ], [ [ 0, 0 ], [ 0, 0 ] ] ] } ]";

			bank.load(input);

			Assert::IsTrue(bank.allGroups.size() == 1);
			Assert::IsTrue(bank.allGroups[0].command == "test");
			Assert::IsTrue(bank.allGroups[0].features.size() == 2);
			Assert::IsTrue(bank.allGroups[0].features[0].size() == 2);
			Assert::IsTrue(bank.allGroups[0].features[0][0].size() == 2);
			Assert::IsTrue(bank.allGroups[0].features[0][1].size() == 2);
		}
	};
}