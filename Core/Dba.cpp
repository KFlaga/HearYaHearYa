#include "Dba.hpp"
#include "Algorithms.hpp"
#include "Expect.hpp"
#include <aquila/aquila.h>
#include <numeric>


/*
DTW:
A, B - input
|A| = S, |B| = T

m[S, T] - matirx of pairs (cost, path)

m[1,1] = (|a[1], b[1]|, (0, 0))
for i = [2, S]
	m[i, 1] = ( m[i-1, 1] + |a[i], b[1]|, (i-1, 1) )
for j = [2, T]
	m[1, j] = ( m[1, j-1] + |a[1], b[i]|, (1, j-1) )

for i = [2, S]
	for j = [2, T]
		min_v = min_element( m[i-1, j], m[i, j-1], m[i-1,j-1] )
		m[i, j] = ( min_v.cost + |a[i], b[i]|, min_v.path )

final_cost = m[S, T]

1. Computing DTW between each individual sequence and the
	temporary average sequence to be refined, in order to find
	associations between coordinates of the average sequence and
	coordinates of the set of sequences.
2. Updating each coordinate of the average sequence as the
	barycenter of coordinates associated to it during the first step.

	We consider the function assoc, that links each coordinate of
	the average sequence to one or more coordinates of the sequences of S.

	This function is computed during DTW computation
	between C and each sequence of S.

	Ck[i] = barycenter(assoc(Ck-1[i]))

	barycenter(x1, .., xn) = sum(x1, .., xn) / n (xi is a vector)

	For now assume all sequences has length T, C has length T'
	assocTab is table of size T' containing in each cell a set of
	coords associated to each coord of C

	m[T,T] is temporary DTW(cost, path) matrix

	Alg:
	for s : S
		m = DTW(C, seq)
		i = T'
		j = T
		while i >= 1, j >= 1
			assocTab[i] = assocTab[i] v(?) seq[j]
			i, j = m[i, j].path

			for i = [1, T]
				Ck+1[i] = barycenter(assocTab[i])

*/

namespace eave
{
	bool isValidAssociation(int s, int p, const std::vector<FeatureVector<MFCC>>& sequences)
	{
		return sequences.size() > s && sequences[s].size() > p;
	}

	const MFCC& findAssociatedFeatures(const DtwAssociation& asc, const std::vector<FeatureVector<MFCC>>& sequences)
	{
		EAVE_EXPECT(isValidAssociation(asc.sequence, asc.point, sequences));
		return sequences[asc.sequence][asc.point];
	}

	MFCC barycenter(const std::vector<DtwAssociation>& associations, const std::vector<FeatureVector<MFCC>>& sequences)
	{
		EAVE_EXPECT(isValidAssociation(0, 0, sequences));

		auto featuresCount = sequences[0][0].size();
		auto result = MFCC( featuresCount, 0.0 );
		for (auto& asc : associations)
		{
			auto& features = findAssociatedFeatures(asc, sequences);
			EAVE_EXPECT(features.size() == featuresCount);

			transformFirstInPlace(result, features, std::plus<double>{});
		}
		transformInPlace(result, divide_by<double>{ double(associations.size()) });
		return result;
	}

	void addAssociations(std::vector<std::vector<DtwAssociation>>& associations, const Aquila::Dtw& dtw, int seqIdx, DbaAssociationsMethod method)
	{
		if (method == DbaAssociationsMethod::AddEachPoint)
		{
			auto& points = dtw.getPoints();
			for (int pos = 0; pos < points.size(); ++pos)
			{
				auto lowerCost = [](const Aquila::DtwPoint& a, const Aquila::DtwPoint& b) { return a.dLocal < b.dLocal; };
				auto bestAssociation = std::min_element(points[pos].begin(), points[pos].end(), lowerCost);
				associations[bestAssociation->y].push_back({ seqIdx, pos, bestAssociation->dLocal });
			}
		}
		else if (method == DbaAssociationsMethod::AddFromPath)
		{
			auto path = dtw.getPath();
			for (auto& p : path)
			{
				// p.x; -> point in sample
				// p.y; -> point in avg
				associations[p.y].push_back({ seqIdx, int(p.x), p.dLocal });
			}
		}
	}

	void computeAverageSequenceWithDBA(const std::vector<FeatureVector<MFCC>>& sequences, FeatureVector<MFCC>& averageSequence, DbaAssociationsMethod method)
	{
		auto associations = std::vector<std::vector<DtwAssociation>>(averageSequence.size());
		for (auto& assoc : associations)
		{
			// Reserve some memory for associations with each element in average sequence
			// Ideally each element would have one association from each sequence
			// So lets reserve such number - maybe it would benefit us a little (todo: revise it after testing in practice)
			assoc.reserve(sequences.size());
		}

		for (int seqIdx = 0; seqIdx < sequences.size(); ++seqIdx)
		{
			auto dtw = Aquila::Dtw{ euclideanDistanceSquared };
			dtw.getDistance(sequences[seqIdx], averageSequence);
			addAssociations(associations, dtw, seqIdx, method);
		}
		transformFirstInPlace(averageSequence, associations,
			[&](auto&, auto& assocForElement) { return barycenter(assocForElement, sequences); });
	}

	FeatureVector<MFCC> pickInitialSequence(const std::vector<FeatureVector<MFCC>>& sequences)
	{
		return sequences[0];
	}

	struct EndCondition
	{
		EndCondition(int maxIterations_, double maxInertia_) :
			maxIterations{ maxIterations_ }, maxInertia{ maxInertia_ }
		{}

		bool isMet()
		{
			return iteration < maxIterations && inertia > maxInertia;
		}

		void update(const std::vector<FeatureVector<MFCC>>& sequences, const FeatureVector<MFCC>& averageSequence)
		{
			inertia = 0.0;
			double sumOfElements = 0.0;
			for (auto& s : sequences)
			{
				inertia += findDTWCost(s, averageSequence);
				sumOfElements = accumulateNested(s.begin(), s.end(), sumOfElements, [](double x, double acc) { return acc + x * x; });
			}

			inertia = std::sqrt(inertia / sumOfElements);
			++iteration;
		}

	private:
		int maxIterations;
		double maxInertia;
		int iteration = 0;
		double inertia = 1.0;
	};

	FeatureVector<MFCC> computeAverageSequenceWithDBA(
		const std::vector<FeatureVector<MFCC>>& sequences,
		DbaAssociationsMethod method,
		double maxRelativeIntertia,
		int maxIterations)
	{
		EAVE_EXPECT(sequences.size() > 0);

		auto averageSequence = pickInitialSequence(sequences);
		auto endCondition = EndCondition{ maxIterations, maxRelativeIntertia };

		while (!endCondition.isMet())
		{
			computeAverageSequenceWithDBA(sequences, averageSequence, method);
			endCondition.update(sequences, averageSequence);
		}

		return averageSequence;
	}
}
