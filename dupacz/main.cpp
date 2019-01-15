// dupacz.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include <map>


/*
{
	"x" : 123,
	"y" : 234
}
*/

struct JsonMap
{
	std::map<std::string, int> dict;
};


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
		// Couldn't open file - for now lets return nothing
		return "";
	}
}


struct State
{
	virtual State* next(char c)
	{
		return this;
	}
};

struct StringState : State
{
	virtual State* next(char c)
	{
		if (c == '"')
		{
			// previous
			return 0;
		}
		else
		{
			value.push_back(c);
		}
	}

	std::string value;
};

struct DictionaryState : State
{
	virtual State* next(char c)
	{
		if (c == '}')
		{
			// previous
			return 0;
		}
		else if (c == '"')
		{
			return new StringState();
		}
		else if (std::isspace(c))
		{
			return this;
		}
		else
		{
			throw std::string("Invalid character");
		}
	}
};

struct StartingState : State
{
	virtual State* next(char c)
	{
		if (c == '{')
		{
			return new DictionaryState();
		}
		else if (std::isspace(c))
		{
			return this;
		}
		else
		{
			throw std::string("Invalid character");
		}
	}
};


JsonMap parseJson(std::string c)
{
	JsonMap result;

	std::stringstream content; // Store content in 'stringstream' as its easy to read parts of text from it
	content << c;

	char next;
	while (not content.eof())
	{
		content >> next;

		if (next == '{')
		{
			// open dict
		}
		else if (next == '}')
		{
			// close dict
		}
		else if (std::isdigit(next))
		{
			// number
		}
		else if (next == '"')
		{
			// string
		}
		else if (std::isspace(next))
		{
			// ws
		}
	}

	return result;
}


int main()
{
	std::string content = readFile("e:/test.txt");
	std::cout << content << "\n";
	parseJson(content);
}
