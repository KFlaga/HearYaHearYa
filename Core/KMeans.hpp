#pragma once

#include "Features.hpp"
#include "Algorithms.hpp"
#include "DeepIterator.hpp"
#include "Expect.hpp"

namespace eave
{
	struct KMeansEndCondition
	{
		KMeansEndCondition(int maxIterations_, double minimumChangeInMeans_ ) :
			maxIterations{ maxIterations_ }, minimumChangeInMeans{ minimumChangeInMeans_ }
		{}

		bool isMet() const
		{
			return iteration < maxIterations && changeInMeans < minimumChangeInMeans;
		}

		void update(const std::vector<MFCC>& oldMeans, const std::vector<MFCC>& currentMeans)
		{
			++iteration;
			double change = 0.0;
			double sum = 0.0;

			forEach2(deepBegin(oldMeans), deepEnd(oldMeans), deepBegin(currentMeans), [&](double x, double y)
			{
				sum += std::abs(x) + std::abs(y);
				change += std::abs(x - y);
			});

			changeInMeans = change / (sum * 0.5);
		}

	private:
		int maxIterations;
		double minimumChangeInMeans;
		int iteration = 0;
		double changeInMeans = 100.0;
	};

	using KMeansInitialPick_t = std::vector<MFCC>(*)(std::size_t, const std::vector<MFCC>&);

	std::vector<MFCC> pickFirstKElements(std::size_t k, const std::vector<MFCC>& data)
	{
		return std::vector<MFCC>(data.begin(), data.begin() + k);
	}

	std::vector<MFCC> findKMeans(
		std::size_t k,
		const std::vector<MFCC>& data,
		KMeansInitialPick_t pickInitialKMeans = pickFirstKElements,
		KMeansEndCondition endCondition = KMeansEndCondition{ 10, 0.01 })
	{
		// Find clusters with k-means
		// Naive version (Lloyd algorithm):
		// Let m1 ... mk be initial set of means
		// Iteratively:
		// - assignment step: assign each observeation to cluster with nearest mean
		// - update step: calculate new means to be centroids of new clusters
		//
		// Initialization:
		// - random pick of k observations
		// - assign each point to random cluster
		// - density based random pick - probability of picking is function of distance to neighbours

		EAVE_EXPECT(data.size() > 0);
		EAVE_EXPECT(data[0].size() > 0);

		std::vector<MFCC> means = pickInitialKMeans(k, data);
		EAVE_EXPECT(means.size() == k);

		std::vector<MFCC> newMeans(k, MFCC(data[0].size())); // Create empty vector for MFCCs
		std::vector<int> counters(k);

		while (!endCondition.isMet())
		{
			// Clear data from previous iteration
			transformInPlace(deepBegin(newMeans), deepEnd(newMeans), [](double) { return 0.0; });
			transformInPlace(std::begin(counters), std::end(counters), [](int) { return 0; });
			// Find nearest mean and add point coords to new cluster centroid
			for (const MFCC& feature : data)
			{
				auto bestMean = findMinCostElement(means.begin(), means.end(), [&](auto& m) { return euclideanDistanceSquared(m, feature); });
				std::size_t bestIndex = std::distance(means.begin(), bestMean);
				transformFirstInPlace(newMeans[bestIndex], feature, std::plus<double>{});
				counters[bestIndex] += 1;
			}
			// Finalize centroid computation for each cluster = sum(x) / N
			forEach2(std::begin(newMeans), std::end(newMeans), std::begin(counters), [](MFCC& mean, int count)
			{
				transformInPlace(mean, divide_by((double)count));
			});

			endCondition.update(means, newMeans);
			std::swap(means, newMeans);
		}

		return means;
	}
}
