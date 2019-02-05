#pragma once

#include "FixedSizeQueue.hpp"
#include "Algorithms.hpp"
#include "Features.hpp"

namespace eave
{
	struct KnnResult
	{
		int group = 0;
		int sample = 0;
		int nnInSameGroup = 0;
		double cost = 10e30;

		bool operator<(const KnnResult& other) const
		{
			return cost < other.cost;
		}

		bool operator>(const KnnResult& other) const
		{
			return cost > other.cost;
		}
	};

	struct CostSquaredDistance
	{
		double operator()(const FeatureVector<double>& sample, const FeatureVector<double>& reference)
		{
			return accumulate2(sample, reference, 0.0, [](double a, double b, double acc) {
				double d = a - b;
				return acc + d * d;
			});
		}

		double operator()(const FeatureVector<MFCC>& sample, const FeatureVector<MFCC>& reference)
		{
			return accumulate2(sample, reference, 0.0, [this](const MFCC& a, const MFCC& b, double acc) {
				return acc + (*this)(a, b);
			});
		}
	};

	using KnnQueue = FixedSizeQueue<KnnResult, std::greater<KnnResult>>;

	// Computes KnnQueue for KNN classifier - k best macthes of "sample" in "reference" groups
	template<typename Feature, typename CostFunction>
	KnnQueue findKnn(
		int k,
		const FeatureVector<Feature>& sample,
		const AllGroups<Feature>& reference,
		CostFunction getCost)
	{
		KnnQueue bestK(k);
		for (int gIdx = 0; gIdx < reference.size(); ++gIdx)
		{
			auto& group = reference[gIdx];
			for (int sIdx = 0; sIdx < group.features.size(); ++sIdx)
			{
				const FeatureVector<Feature>& x = group.features[sIdx];
				double cost = getCost(sample, x);
				bestK.push({ gIdx, sIdx, 0, cost });
			}
		}

		return bestK;
	}

	// Finds best matched group according to given KnnQueue
	inline int classifyKnn(KnnQueue& knn)
	{
		forEachWithEach(knn.begin(), knn.end(), IncludeSelf{}, [](auto& a, auto& b) {
			a.nnInSameGroup += int(a.group == b.group);
		});

		auto compareNnInGroup = [](auto& a, auto& b) { return a.nnInSameGroup < b.nnInSameGroup; };
		return std::max_element(knn.begin(), knn.end(), compareNnInGroup)->group;
	}

	// Classifes given "sample" object to one of groups in "reference" using K Nearest Neighbours classifier
	template<typename Feature, typename CostFunction>
	int classifyKnn(
		int k,
		const FeatureVector<Feature>& sample,
		const AllGroups<Feature>& reference,
		CostFunction getCost)
	{
		KnnQueue knn = findKnn(k, sample, reference, getCost);
		return classifyKnn(knn);
	}

	// Classifes given "sample" object to one of groups in "reference" by finding nearest match
	template<typename Feature, typename CostFunction>
	int classifyNearest(
		const FeatureVector<Feature>& sample,
		const std::vector<FeatureVector<Feature>>& referenceAverages,
		CostFunction getCost)
	{
		auto minCostElement = findMinCostElement(std::begin(referenceAverages), std::end(referenceAverages),
			[&](auto& average) { return getCost(average, sample); }
		);
		return std::distance(std::begin(referenceAverages), minCostElement);
	}
}
