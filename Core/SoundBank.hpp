#pragma once

#include "Json.hpp"

namespace Aquila
{
	class SignalSource;
	class WaveFile;
}

namespace eave
{
	struct SoundsGroup
	{
		std::string command;
		std::string directory;
		std::vector<std::string> files;
	};

	struct SoundBank
	{
		SoundBank(JsonDocument& json);
		~SoundBank();

		Aquila::SignalSource& loadSound(int group, int soundIndex);
		Aquila::SignalSource& loadSound(const std::string& fileName, const std::string& directory);

		std::vector<SoundsGroup> allGroups;
		Aquila::WaveFile* wavFile = nullptr;
	};
}
