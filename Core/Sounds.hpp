#pragma once

#include "Json.hpp"

#include <aquila/aquila.h>
#include <algorithm>

#include <Windows.h>

namespace eave
{

	Aquila::WaveFile loadWav(const std::string& path)
	{
		Aquila::WaveFile wavFile{ path };
		return wavFile;
	}

	std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	std::string ws2s(const std::wstring& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
		char* buf = new char[len];
		WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
		std::string r(buf);
		delete[] buf;
		return r;
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
		HANDLE hFind;
		WIN32_FIND_DATA data;

		auto win32Str = getAbsoultePath(s2ws(directory)) + L"*";  // convert to win32 format
		std::vector<std::string> files; // result vector with file names

		hFind = FindFirstFile(win32Str.c_str(), &data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				std::wstring fileName = data.cFileName;
				files.push_back(ws2s(fileName)); // add new element to vector
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
		return files;
	}

	std::vector<std::string> filterInWavFiles(const std::vector<std::string>& files)
	{
		std::vector<std::string> result;

		for (const std::string& fileName : files)
		{
			std::size_t pos = 0;
			while (fileName[pos] != '.') { ++pos; }; // find dot
			pos++; // skip dot

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

	struct SoundBank
	{
		SoundBank(JsonDocument& json)
		{
			JsonMap& root = json.root().asMap();

			JsonList& commands = root["voice_commands"].asList();

			for (JsonElement* x : commands.value)
			{
				JsonMap& element = x->asMap();
				SoundsGroup sounds;
				sounds.command = element["name"].asString();
				sounds.directory = element["train_sounds_dir"].asString();

				std::vector<std::string> allFiles = findAllFilesInDirectory(sounds.directory);
				sounds.files = filterInWavFiles(allFiles);

				allGroups.push_back(sounds);
			}
		}

		Aquila::SignalSource& loadSound(int group, int soundIndex)
		{
			if (wavFile != nullptr)
			{
				delete wavFile;
			}

			std::string path = allGroups[group].directory + "/" + allGroups[group].files[soundIndex];
			wavFile = new Aquila::WaveFile(path);
			return *wavFile;
		}

		struct SoundsGroup
		{
			std::string command;
			std::string directory;
			std::vector<std::string> files;
		};

		std::vector<SoundsGroup> allGroups;
		Aquila::WaveFile* wavFile = nullptr;
	};
}
