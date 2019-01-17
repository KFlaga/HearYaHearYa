#include "Json_2.hpp"

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
		// Couldn't open file
		throw std::runtime_error("Couldn't open file");
	}
}


int main()
{
	std::string content = readFile("SoundBank.json");
	std::cout << content << "\n";
	eave::JsonElement* jsonPtr = eave::parseJson(content);

	eave::JsonElement& json = *jsonPtr;

	std::cout << "PARSED\n";
	std::cout << json.str() << "\n";
	std::cout << json["x"].get<eave::JsonInteger>().value << "\n";
}
