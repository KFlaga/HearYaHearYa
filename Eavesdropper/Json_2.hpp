#pragma once

#include "Input.hpp"

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>

#include <iostream>


namespace eave
{
	struct JsonElement
	{
		virtual std::string str() = 0;

		virtual JsonElement& operator[](int)
		{
			throw std::runtime_error("Operation not supported");
		}

		virtual JsonElement& operator[](const std::string&)
		{
			throw std::runtime_error("Operation not supported");
		}

		template<typename Elem>
		Elem& get()
		{
			return dynamic_cast<Elem&>(*this);
		}
	};

	struct JsonInteger : JsonElement
	{
		JsonInteger(int x) : value{ x } {}

		std::string str() override
		{
			return std::to_string(value);
		}

		int value;
	};

	struct JsonString : JsonElement
	{
		JsonString(const std::string& x) : value{ x } {}

		std::string str() override
		{
			return value;
		}

		std::string value;
	};

	struct JsonMap : JsonElement
	{
		~JsonMap() // destructor
		{
			for (auto item : value)
			{
				delete item.second;
			}
		}

		std::string str() override
		{
			bool isFirst = true;

			std::stringstream ss;
			ss << "{ ";
			for (auto& x : value)
			{
				if (! isFirst)
				{
					ss << ", ";
				}
				ss << x.first << " : " << x.second->str();
			}
			ss << " }";
			return ss.str();
		}

		JsonElement& operator[](const std::string& key) override
		{
			return *value.at(key);
		}

		std::map<std::string, JsonElement*> value;
	};

	struct JsonList : JsonElement
	{
		std::string str() override
		{
			return ""; // TODO
		}

		JsonElement& operator[](int index) override
		{
			return *value.at(index);
		}

		std::vector<JsonElement*> value;
	};

	JsonElement* parseJson(Input& input);

	void skipSpace(Input& input)
	{
		input.readWhile(std::isspace);
	}

	void expectColon(Input& input)
	{
		if(input.get() != ':')
		{
			throw std::runtime_error("Expected colon");
		}
		input.next();
	}

	bool maybeComma(Input& input)
	{
		if (input.get() == ',')
		{
			input.next();
			return true;
		}
		return false;
	}

	JsonInteger* parseInteger(Input& input)
	{
		// Zalozenia wejsciowe: jestesmy przy cyfrze
		int value = std::stoi(input.readWhile(std::isdigit));
		return new JsonInteger(value);
	}

	bool isNotQuote(char c)
	{
		return c != '"';
	}

	JsonString* parseString(Input& input)
	{
		// Zalozenia wejsciowe: jestesmy przy '"'

		if (input.get() != '"')
		{
			throw std::runtime_error("Expected quote");
		}

		input.next(); // Skip quote
		std::string temp = input.readWhile(isNotQuote);
		input.next(); // Skip quote

		return new JsonString(temp);
	}

	JsonMap* parseMap(Input& input)
	{
		// Zalozenia wejsciowe: jestesmy przy '{'

		if (input.get() != '{')
		{
			// niedobrze
			throw std::runtime_error("Expected klamerka");
		}
		input.next(); // Skip curly-bracket

		JsonMap* result = new JsonMap();

		skipSpace(input);
		if (input.get() == '}')
		{
			input.next();
			return result;
		}

		do
		{
			skipSpace(input);

			JsonString* key = parseString(input);

			skipSpace(input);
			expectColon(input);
			skipSpace(input);

			JsonElement* element = parseJson(input);

			result->value[key->value] = element;

			delete key;

			skipSpace(input);
		} while (maybeComma(input));

		if (input.get() != '}')
		{
			throw std::runtime_error("Expected klamerka druga");
		}
		input.next(); // Skip curly-bracket

		return result;
	}


	JsonList* parseList(Input& input)
	{
		JsonList* result = new JsonList();

		// TODO
		/*
		[ <- otwiera list
		 1, "2", [3] <- elementy z przecinkami
		] <- zamyka liste

		[1, 2, 3]
		[]
		["aa", "tt", { }]

		WRITE TESTS
		*/

		return result;
	}

	JsonElement* parseJson(Input& input)
	{
		skipSpace(input);
		if (std::isdigit(input.get()))
		{
			return parseInteger(input);
		}
		else if(input.get() != '"')
		{
			return parseString(input);
		}
		else if (input.get() != '{')
		{
			return parseMap(input);
		}
		else
		{
			throw std::runtime_error("Unexpected character");
		}
	}

	JsonElement* parseJson(const std::string& input)
	{
		Input x(input);
		return parseJson(x);
	}
}
