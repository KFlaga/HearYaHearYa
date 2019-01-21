#pragma once

#include "FixedSizeQueue.hpp"
#include "Algorithms.hpp"
#include <vector>
#include <functional>

namespace eave
{
	template<typename Feature>
	using FeatureVector = std::vector<Feature>;

	template<typename Feature>
	using SampleGroup = std::vector<FeatureVector<Feature>>;

	template<typename Feature>
	using AllGroups = std::vector<SampleGroup<Feature>>;

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
	};

	using KnnQueue = FixedSizeQueue<KnnResult, std::greater<KnnResult>>;

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
			const SampleGroup<Feature>& group = reference[gIdx];
			for (int sIdx = 0; sIdx < group.size(); ++sIdx)
			{
				const FeatureVector<Feature>& x = group[sIdx];
				double cost = getCost(sample, x);
				bestK.push({ gIdx, sIdx, 0, cost });
			}
		}

		return bestK;
	}

	inline int classifyKnn(KnnQueue& knn)
	{
		forEachWithEach(knn.begin(), knn.end(), IncludeSelf{}, [](auto& a, auto& b) {
			a.nnInSameGroup += int(a.group == b.group);
		});

		auto compareNnInGroup = [](auto& a, auto& b) { return a.nnInSameGroup < b.nnInSameGroup; };
		return std::max_element(knn.begin(), knn.end(), compareNnInGroup)->group;
	}

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
}
