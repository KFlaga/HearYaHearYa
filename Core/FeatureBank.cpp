#include "Features.hpp"
#include "Json.hpp"

#include <aquila/aquila.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace eave
{
	double greaterEqualPowerOf2(double x)
	{
		double i = 1.0;
		while (i <= x)
		{
			i *= 2.0;
		}
		return i;
	}

	FeatureBank::FeatureBank(
		SoundBank& sounds,
		double frameLengthMs,
		double overlap) // In range [0-1]
	{
		// Iterate over all groups in SoundBank
		for (int g = 0; g < sounds.allGroups.size(); ++g)
		{
			// For each sound group - crate FeatureGroup
			const SoundsGroup& group = sounds.allGroups[g];

			FeatureGroup featureGroup;
			featureGroup.command = group.command;

			for (int s = 0; s < group.files.size(); ++s)
			{
				// For each sound in sounds group - find features for sound
				Aquila::SignalSource& sound = sounds.loadSound(g, s);
				FeatureVector<MFCC> featuresForSound;

				// Split into frames
				// Aquila requires for frame size to be power of 2
				double samplesInFrame = frameLengthMs * 0.001 * sound.getSampleFrequency();
				unsigned int samplesInFrame_pow2 = (unsigned int)greaterEqualPowerOf2(samplesInFrame);
				unsigned int samplesInOverlap = (unsigned int)samplesInFrame_pow2 * overlap;

				Aquila::FramesCollection frames{ sound, samplesInFrame_pow2, samplesInOverlap };
				std::size_t featuresCount = 12; // 12 is standard according to smart guys

				// For each frame find frame's features with Mfcc
				Aquila::Mfcc mfcc{ samplesInFrame_pow2 };

				for (auto& frame : frames)
				{
					MFCC frameFeatures = mfcc.calculate(frame, featuresCount);
					featuresForSound.push_back(frameFeatures);
				}

				// Add found features for sound to group
				featureGroup.features.push_back(featuresForSound);
			}

			// Add created group to all groups
			allGroups.push_back(featureGroup);
		}
	}

	void FeatureBank::save(std::ostream& file)
	{
		/*
		Example result:
		[
			{
				"command" : "xxx",
				"features" : [
					[ [ 1.0, 2.0, 3.0 ], [ 1.0, 2.0, 3.0 ] ]
				]
			}
		]
		We need to map data from "allGroups" onto Json structures
		*/

		JsonList jsonAllGroups;

		// For each sound-group create JsonMap and fill it with data
		for (int i = 0; i < allGroups.size(); ++i)
		{
			// We create helper variable "group" as its more meaningful than "allGroups[i]" (at least it is for me)
			const FeatureGroup& group = allGroups[i];

			// We create "new" JsonMap - which means we put it on heap, so that it will outlive current scope
			// If we would just create local object ( "JsonMap jsonGroup" ) then after leaving scope (here - "for" loop)
			// it would be destroyed and unsuable. And we need to use it after the loop.
			JsonMap* jsonGroup = new JsonMap();
			jsonAllGroups.push_back(jsonGroup); // Lets add JsonMap related to current "group" to list of all groups

			// Add "command" entry to group
			JsonString* jsonCommand = new JsonString(group.command);
			jsonGroup->insert(std::make_pair("command", jsonCommand));

			// Then add "features" entry - related to "group.features" (which is list of features for each track in group)
			// First create list for this and fill it with data - we want to add element for
			// each element from "group.features"
			JsonList* jsonFeatures = new JsonList();

			for (int k = 0; k < group.features.size(); ++k)
			{
				const FeatureVector<MFCC>& features = group.features[k];

				JsonList* jsonFeatureVector = new JsonList();

				for (int l = 0; l < features.size(); ++l)
				{
					const MFCC& mfcc = features[k];
					JsonList* jsonMFCC = new JsonList();

					for (int p = 0; p < mfcc.size(); ++p)
					{
						// We are finally at last level with actual numbers - add each of them to list
						JsonInteger* jsonInt = new JsonInteger{ int(mfcc[p]) };
						jsonMFCC->push_back(jsonInt);
					}

					// Now, as list is filled with data lets add it
					jsonFeatureVector->push_back(jsonMFCC);
				}

				// Same as above
				jsonFeatures->push_back(jsonFeatureVector);
			}

			// Same as above, but add to map as entry with key "features"
			jsonGroup->insert(std::make_pair("features", jsonFeatures));
		}

		// We have all data stored in Json structure
		// Obtain textual representation of this strucutre with "str()" and put it in file
		file << jsonAllGroups.str();
	}

	void FeatureBank::saveToFile(const std::string& path)
	{
		// open file - ofstream as its output
		std::ofstream file(path);

		bool isOpen = file.is_open();
		if (isOpen)
		{
			save(file);
			file.close();
		}
		else
		{
			throw std::runtime_error("Couldn't open file");
		}
	}

	void FeatureBank::load(std::istream& file)
	{
		// We use std::istream, so it will accept files (ifstream) but also other streams (like stringstream)
		throw std::runtime_error("Implement me!");
	}

	void FeatureBank::loadFromFile(const std::string& path)
	{
		// open file - ifstream as its input
		std::ifstream file(path);

		bool isOpen = file.is_open();
		if (isOpen)
		{
			load(file);
			file.close();
		}
		else
		{
			throw std::runtime_error("Couldn't open file");
		}
	}

	double findDTWCost(const FeatureVector<MFCC>& from, const FeatureVector<MFCC>& to)
	{
		Aquila::Dtw dtw{ euclideanDistanceSquared };
		double distance = dtw.getDistance(from, to);
		return distance;
	}
}
