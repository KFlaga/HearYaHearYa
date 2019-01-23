#pragma once

#include <vector>
#include <functional>
#include <ostream>
#include <istream>

#include "SoundBank.hpp"

namespace eave
{
	using MFCC = std::vector<double>;

	template<typename Feature>
	using FeatureVector = std::vector<Feature>;

	struct FeatureGroup
	{
		std::string command;
		std::vector<FeatureVector<MFCC>> features;
	};

	struct FeatureBank
	{
		FeatureBank() {};

		FeatureBank(
			SoundBank& sounds,
			double frameLengthMs = 10.0,
			double overlap = 0.0 // In [0.0 - 1.0]
		);

		void saveToFile(const std::string& path);
		void save(std::ostream& source);

		void loadFromFile(const std::string& path);
		void load(std::istream& source);

		std::vector<FeatureGroup> allGroups;
	};

	template<typename Feature>
	using AllGroups = std::vector<FeatureGroup>;

	double findDTWCost(const FeatureVector<MFCC>& a, const FeatureVector<MFCC>& b);
}
