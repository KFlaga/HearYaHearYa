#include "SoundBank.hpp"

#include <aquila/aquila.h>
#include <algorithm>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <locale>
#include <codecvt>
#include <string>

#include <Windows.h>

namespace eave
{
	// Converts 8bit char string to wide char string
	std::wstring convertToWide(const std::string& s)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(s);
	}

	// Converts wide char string char string to 8bit char string
	std::string convertToNormal(const std::wstring& s)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(s);
	}

	std::wstring getAbsoultePath(const std::wstring& path)
	{
		DWORD  retval = 0;
		BOOL   success;
		TCHAR  buffer[MAX_PATH] = TEXT("");
		TCHAR  buf[MAX_PATH] = TEXT("");
		LPWSTR* lppPart = { NULL };

		retval = GetFullPathName(path.c_str(), MAX_PATH, buffer, lppPart);

		if (retval == 0)
		{
			throw std::runtime_error("getAbsoultePath - failed");
		}
		else
		{
			std::wstring absPath = buffer;
			return absPath;
		}
	}

	std::vector<std::string> findAllFilesInDirectory(const std::string& directory)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

		HANDLE hFind;
		WIN32_FIND_DATA data;

		auto win32Str = getAbsoultePath(convertToWide(directory)) + L"*";  // convert to win32 format
		std::vector<std::string> files; // result vector with file names

		hFind = FindFirstFile(win32Str.c_str(), &data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do // For each file in directory push it to "files"
			{
				std::wstring fileName = data.cFileName;
				files.push_back(convertToNormal(fileName)); // add new element to vector
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
		return files;
	}

	std::vector<std::string> filterInWavFiles(const std::vector<std::string>& files) // todo: make general filterIn
	{
		std::vector<std::string> result;

		for (const std::string& fileName : files)
		{
			std::size_t pos = 0;
			while (fileName[pos] != '.') { ++pos; }; // find dot
			pos++; // skip dot

			// Find extension
			std::string extension;
			for (; pos < fileName.size(); ++pos)
			{
				extension.push_back(fileName[pos]);
			}

			if (extension == "wav")
			{
				result.push_back(fileName);
			}
		}

		return result;
	}

	SoundBank::SoundBank(JsonDocument& json)
	{
		/*
		Example:
		{
			"voice_commands": [
				{
					"name": "test_sound",
					"train_sounds_dir" : "input/test_sound/"
				}
			]
		}
		We need to convert Json structure with textual representation as above to SoundBank data structure
		*/

		// First element is map with entry "voice_commands"
		// Value for "voice_commands" is a list of maps which represent "SoundsGroup",
		// so for each element from this list we need to create "SoundsGroup" and fill it with data
		// from related JsonMap.

		JsonMap& root = json.root().asMap();
		JsonList& commands = root["voice_commands"].asList();

		for (JsonElement* x : commands)
		{
			JsonMap& element = x->asMap();
			SoundsGroup sounds;
			sounds.command = element["name"].asString();
			sounds.directory = element["train_sounds_dir"].asString();

			// Find all ".wav" files in just read directory and store them in "sound.files"
			std::vector<std::string> allFiles = findAllFilesInDirectory(sounds.directory);
			sounds.files = filterInWavFiles(allFiles);

			// We have filled "SoundsGroup" with data, so we can add it to all groups
			allGroups.push_back(sounds);
		}
	}

	SoundBank::~SoundBank()
	{
		if (wavFile != nullptr)
		{
			delete wavFile;
		}
	}

	Aquila::SignalSource& SoundBank::loadSound(const std::string& fileName, const std::string& directory)
	{
		// We allow for only one file to be loaded at a time, so if there is one loaded
		// (as indicated by "!= nullptr") delete it (it will remove it from used memory).
		if (wavFile != nullptr)
		{
			delete wavFile;
		}

		// Find file with given group/sound index and append group directory to file name, then load it and store
		std::string path = directory + "/" + fileName;
		wavFile = new Aquila::WaveFile(path);
		return *wavFile;
	}

	Aquila::SignalSource& SoundBank::loadSound(int group, int soundIndex)
	{
		return loadSound(allGroups[group].files[soundIndex], allGroups[group].directory);
	}
}
