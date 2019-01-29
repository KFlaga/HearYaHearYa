#pragma once

#include "Features.hpp"

namespace eave
{
	struct DtwAssociation
	{
		int sequence;
		int point;
		double cost;
	};

	/*
	 * Computes barycenter (vector mean) of all sequences elements given mapping "associations".
	 * For each association finds corresponding element in "sequences" and accumulates its features
	 * then divides resulting sum by number of associations.
	 */
	MFCC barycenter(const std::vector<DtwAssociation>& associations, const std::vector<FeatureVector<MFCC>>& sequences);

	enum class DbaAssociationsMethod
	{
		AddEachPoint,
		AddFromPath
	};

	void computeAverageSequenceWithDBA(
		const std::vector<FeatureVector<MFCC>>& sequences,
		FeatureVector<MFCC>& initialAverage,
		DbaAssociationsMethod method = DbaAssociationsMethod::AddEachPoint);

	/*
	 * Computes average sequence for given list of sequences using DBA method (DTW barycenter averaging).
	 * 
	 */
	FeatureVector<MFCC> computeAverageSequenceWithDBA(
		const std::vector<FeatureVector<MFCC>>& sequences,
		DbaAssociationsMethod method = DbaAssociationsMethod::AddEachPoint,
		double maxRelativeIntertia = 0.1,
		int maxIterations = 10);
}
