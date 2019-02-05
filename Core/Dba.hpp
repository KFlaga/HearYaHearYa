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

	/*
	 * End condition for DBA algorithm.
	 * Allows to end after reaching given interation number or interclass inertia of computed average.
	 * Inertia is computed as sum(DTW(average, sequences[i])) / sum(sequences[i]^2).
	 * (todo: it is assumed that DTW is computed with squared euclidean distance, maybe parameterize it)
	 * todo: consider if raising inertia after update should trigger end condition (after length-reduction of average is added)
	 */
	struct DbaEndCondition
	{
		DbaEndCondition(int maxIterations_, double maxRelativeInertia_) :
			maxIterations{ maxIterations_ }, maxInertia{ maxRelativeInertia_ }
		{}

		bool isMet() const
		{
			return iteration < maxIterations && inertia > maxInertia;
		}

		void update(const std::vector<FeatureVector<MFCC>>& sequences, const FeatureVector<MFCC>& averageSequence);

	private:
		int maxIterations;
		double maxInertia;
		int iteration = 0;
		double inertia = 1.0;
	};

	// TODO: revise if both of those methods makes sense after testing on real data
	enum class DbaAssociationsMethod
	{
		AddEachPoint,
		AddFromPath
	};

	/*
	 * Computes average sequence for given list of sequences using DBA method (DTW barycenter averaging).
	 * Performs single iteration of algorithm.
	 *
	 * Idea taken from: "A global averaging method for dynamic time warping, with applications to clustering"
	 * by Francois Petitjean, Alain Ketterlin and Pierre Gancarski
	 *
	 */
	void computeAverageSequenceWithDBA(
		const std::vector<FeatureVector<MFCC>>& sequences,
		FeatureVector<MFCC>& initialAverage,
		DbaAssociationsMethod method = DbaAssociationsMethod::AddEachPoint);

	/*
	 * Computes average sequence for given list of sequences using DBA method.
	 * Performs interations of average updating until end condition is not met.
	 */
	FeatureVector<MFCC> computeAverageSequenceWithDBA(
		const std::vector<FeatureVector<MFCC>>& sequences,
		DbaAssociationsMethod method = DbaAssociationsMethod::AddEachPoint,
		DbaEndCondition endCondition = DbaEndCondition{ 10, 0.1 });

	/*
	 * Shortens given sequence by removing elements with lowest "fitness", i.e. lowest count of assignments weighted by their cost.
	 * Formula for picking element k for merging is max{k} : sum{i=[1,N]}(assoc_cost[k][i]) / N^p, where p is adjustable parameter (~1.5 - 2.5).
	 * When p rises, elements with lowest number associations are more preffered rather than with highest cost.
	 * It doesn't have strong justification, its just an educated guess.
	 */
	FeatureVector<MFCC> removeWorstElementInAverageSequence(
		const FeatureVector<MFCC>& average,
		const std::vector<std::vector<DtwAssociation>>& associations,
		double p = 2.0);

	/*
	 * Removes elements from sequence without any associations - such elements are of little use
	 */
	FeatureVector<MFCC> removeElementsWithoutAssociations(
		const FeatureVector<MFCC>& average,
		const std::vector<std::vector<DtwAssociation>>& associations);

	/*
	 * Computess "fitness" of given average sequence element, that is:
	 * -sum{i=[1,N]}(assoc_cost[i]) / N^p
	 */
	double computeFitness(const std::vector<DtwAssociation>& associations, double p);
}
