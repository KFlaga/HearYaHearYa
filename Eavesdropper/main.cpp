#include "Json.hpp"
#include "AquilaTest.hpp"
#include "Sounds.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>


std::string readFile(std::string fileName)
{
	// First we create file stream. File is opened in 'std::ifstream' constructor.
	std::ifstream file(fileName);

	bool isOpen = file.is_open();
	if (isOpen)
	{
		std::stringstream content; // 'std::stringstream' is used to append text multiple time
		std::string buf; // Temporary buffer
		while (not file.eof())
		{
			// Read line by line until EndOfFile is reached.
			// Each line is stored in temporary 'buf' and then appended to 'content'
			std::getline(file, buf);
			content << buf << "\n";
		}
		file.close(); // Lets close file as it is no longer used
		return content.str();
	}
	else
	{
		throw std::runtime_error("Couldn't open file");
	}
}

struct Command
{
	std::string name;
	std::string samplesDir;
};

int main()
{
	using namespace eave;

	std::string content = readFile("SoundBank.json");
	std::cout << content << "\n";

	try
	{
		JsonDocument json(content);

		SoundBank bank(json);

		std::cout << bank.allGroups.size() << "\n";

		auto& wavFile = bank.loadSound(0, 0);

		std::cout << wavFile.getSamplesCount() << "\n";
	}
	catch (const std::runtime_error& e)
	{
		std::cout << "Failed to parse SoundBank.json. Reason: " << e.what() << "\n";
	}

	//testLoadWav();
}
