#include "stdafx.h"
#include "CppUnitTest.h"

#include <Dba.hpp>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	TEST_CLASS(DbaTests)
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

		TEST_METHOD(computeBarycenter_oneSequence)
		{
			std::vector<DtwAssociation> associations{
				DtwAssociation{0, 0, 0.0},
				DtwAssociation{0, 1, 0.0},
				DtwAssociation{0, 3, 0.0},
			};
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 1.0, 2.0, 3.0 },
					MFCC{ 2.0, 5.0, -3.0 },
					MFCC{ -91.0, 128.0, 22.0 },
					MFCC{ 0.0, 1.0, 4.0 }
				}
			};

			MFCC result = barycenter(associations, sequences);
			MFCC expected{ 3.0 / 3.0, 8.0 / 3.0, 4.0 / 3.0 };

			Assert::IsTrue(expected == result);
		}

		TEST_METHOD(computeBarycenter_threeSequences)
		{
			std::vector<DtwAssociation> associations{
				DtwAssociation{0, 0, 0.0},
				DtwAssociation{0, 1, 0.0},
				DtwAssociation{1, 1, 0.0},
				DtwAssociation{2, 0, 0.0},
			};
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 1.0, 2.0 },
					MFCC{ 2.0, 5.0 }
				},
				FeatureVector<MFCC>{
					MFCC{ 4.0, 6.0 },
					MFCC{ 2.0, -1.0 }
				},
				FeatureVector<MFCC>{
					MFCC{ 4.0, 0.0 },
					MFCC{ -3.0, -1.0 }
				}
			};

			MFCC result = barycenter(associations, sequences);
			MFCC expected{ 9.0 / 4.0, 6.0 / 4.0 };

			Assert::IsTrue(expected == result);
		}

		TEST_METHOD(computeBarycenter_emptyAssociations)
		{
			std::vector<DtwAssociation> associations{};
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 1.0, 2.0, 3.0 }
				}
			};

			MFCC result = barycenter(associations, sequences);

			Assert::IsTrue(result.size() == 3);
			Assert::IsTrue(isnan(result[0]));
			Assert::IsTrue(isnan(result[1]));
			Assert::IsTrue(isnan(result[2]));
		}

		TEST_METHOD(computeBarycenter_invalidInput_unequalFeatureLengths)
		{
			std::vector<DtwAssociation> associations{
				DtwAssociation{0, 0, 0.0},
				DtwAssociation{1, 1, 0.0},
			};
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 1.0, 2.0 },
					MFCC{ 2.0, 5.0, 4.0 }
				},
				FeatureVector<MFCC>{
					MFCC{ 4.0, 6.0 },
					MFCC{ 2.0 }
				}
			};

			expectThrow([&]() { barycenter(associations, sequences); });
		}

		TEST_METHOD(computeBarycenter_invalidInput_emptySequences)
		{
			std::vector<DtwAssociation> associations{};
			std::vector<FeatureVector<MFCC>> sequences{};

			expectThrow([&]() { barycenter(associations, sequences); });
		}

		TEST_METHOD(computeDba_oneSequence)
		{
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 1.0, 2.0, 3.0 },
					MFCC{ 2.0, 5.0, -3.0 },
					MFCC{ 0.0, 1.0, 4.0 }
				}
			};

			FeatureVector<MFCC> initial = sequences[0];
			computeAverageSequenceWithDBA(sequences, initial);

			Assert::IsTrue(initial == sequences[0]);
		}

		TEST_METHOD(computeDba_twoSequences_aligned)
		{
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 0.0, 2.0, 10.0 },
					MFCC{ -2.0, 7.0, -10.0 },
					MFCC{ 12.0, 22.0, 100.0 }
				},
				FeatureVector<MFCC>{
					MFCC{ 2.0, 4.0, 8.0 },
					MFCC{ -6.0, 9.0, -12.0 },
					MFCC{ 16.0, 24.0, 106.0 }
				}
			};

			FeatureVector<MFCC> expected{
				MFCC{ 1.0, 3.0, 9.0 },
				MFCC{ -4.0, 8.0, -11.0 },
				MFCC{ 14.0, 23.0, 103.0 }
			};

			FeatureVector<MFCC> initial = sequences[0];
			computeAverageSequenceWithDBA(sequences, initial);

			Assert::IsTrue(initial == expected);
		}

		TEST_METHOD(computeDba_twoSequences_shifted)
		{
			std::vector<FeatureVector<MFCC>> sequences{
				FeatureVector<MFCC>{
					MFCC{ 0.0, 1.0, 2.0 },
					MFCC{ 1.0, 1.0, 3.0 },
					MFCC{ 1.0, 2.0, 3.0 },
					MFCC{ 2.0, 3.0, 3.0 },
				},
				FeatureVector<MFCC>{
					MFCC{ -1.0, 1.0, 1.0 },
					MFCC{ 0.0, 1.0, 1.0 },
					MFCC{ 0.0, 1.0, 2.0 },
					MFCC{ 1.0, 1.0, 3.0 },
				}
			};

			{
				FeatureVector<MFCC> initial = sequences[0];

				FeatureVector<MFCC> expected{
					MFCC{ -1.0 / 4.0, 4.0 / 4.0, 6.0 / 4.0 },
					MFCC{ 1.0, 1.0, 3.0 },
					MFCC{ 1.0, 2.0, 3.0 },
					MFCC{ 2.0, 3.0, 3.0 },
				};

				computeAverageSequenceWithDBA(sequences, initial);
				Assert::IsTrue(initial == expected);
			}
			{
				FeatureVector<MFCC> initial = sequences[1];

				FeatureVector<MFCC> expected{
					MFCC{ -1.0, 1.0, 1.0 },
					MFCC{ 0.0, 1.0, 1.0 },
					MFCC{ 0.0, 1.0, 2.0 },
					MFCC{ 5.0 / 4.0, 7.0 / 4.0, 12.0 / 4.0 },
				};

				computeAverageSequenceWithDBA(sequences, initial);
				Assert::IsTrue(initial == expected);
			}
		}

		TEST_METHOD(test_removeWorstElementInAverageSequence)
		{
			std::vector<std::vector<DtwAssociation>> associations
			{
				std::vector<DtwAssociation>
				{
					DtwAssociation{0, 0, 1.0},
					DtwAssociation{0, 1, 3.0},
					DtwAssociation{0, 2, 2.0} // fitness = -0.67
				},
				std::vector<DtwAssociation>
				{
					DtwAssociation{1, 0, 4.0},
					DtwAssociation{1, 1, 3.0},
					DtwAssociation{1, 2, 0.0} // fitness = -0.78
				},
				std::vector<DtwAssociation>
				{
					DtwAssociation{2, 0, 1.0},
					DtwAssociation{2, 1, 2.0}, // fitness = -0.75
				},
			};

			FeatureVector<MFCC> average{ MFCC{1.0}, MFCC{2.0}, MFCC{0.0} };
			FeatureVector<MFCC> expected{ MFCC{1.0},  MFCC{0.0} };

			FeatureVector<MFCC> result = removeWorstElementInAverageSequence(average, associations, 2.0);
			Assert::IsTrue(result == expected);

			associations[0].erase(associations[0].begin());
			expected = { MFCC{2.0},  MFCC{0.0} };

			result = removeWorstElementInAverageSequence(average, associations, 1.0);
			Assert::IsTrue(result == expected);
		}

		TEST_METHOD(test_removeElementsWithoutAssociations)
		{
			std::vector<std::vector<DtwAssociation>> associations
			{
				std::vector<DtwAssociation>
				{
					DtwAssociation{0, 0, 1.0},
					DtwAssociation{1, 1, 3.0},
					DtwAssociation{0, 1, 2.0}
				},
				std::vector<DtwAssociation>{},
				std::vector<DtwAssociation>
				{
					DtwAssociation{1, 0, 3.0}
				},
				std::vector<DtwAssociation>{}
			};

			FeatureVector<MFCC> average{ MFCC{1.0}, MFCC{2.0}, MFCC{0.0}, MFCC{-2.0} };
			FeatureVector<MFCC> expected{ MFCC{1.0},  MFCC{0.0}};

			FeatureVector<MFCC> result = removeElementsWithoutAssociations(average, associations);

			Assert::IsTrue(result == expected);
		}

		TEST_METHOD(test_computeFitness)
		{
			std::vector<DtwAssociation> associations{
				DtwAssociation{0, 0, 1.0},
				DtwAssociation{1, 1, 3.0},
				DtwAssociation{0, 1, 2.0},
			};

			Assert::AreEqual(-6.0 / 3.0, computeFitness(associations, 1.0));
			Assert::AreEqual(-6.0 / 9.0, computeFitness(associations, 2.0));
		}
	};
}
