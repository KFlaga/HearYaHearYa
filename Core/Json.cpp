#pragma once

#include "Json.hpp"

#include <sstream>
#include <cctype>

#include <iostream>


namespace eave
{
	JsonInteger& JsonElement::asInteger()
	{
		expect(JsonElementType::Integer);
		return static_cast<JsonInteger&>(*this);
	}

	JsonString& JsonElement::asString()
	{
		expect(JsonElementType::String);
		return static_cast<JsonString&>(*this);
	}

	JsonMap& JsonElement::asMap()
	{
		expect(JsonElementType::Map);
		return static_cast<JsonMap&>(*this);
	}

	JsonList& JsonElement::asList()
	{
		expect(JsonElementType::List);
		return static_cast<JsonList&>(*this);
	}

	JsonDouble& JsonElement::asDouble()
	{
		expect(JsonElementType::Double);
		return static_cast<JsonDouble&>(*this);
	}

	void JsonElement::expect(JsonElementType x)
	{
		if (type_ != x)
		{
			throw std::runtime_error("Invalid json cast");
		}
	}

	std::string JsonMap::str()
	{
		bool isFirst = true;

		std::stringstream ss;
		ss << "{ ";
		for (auto& x : value)
		{
			if (!isFirst)
			{
				ss << ", ";
			}
			isFirst = false;
			ss << '"' << x.first << '"' << " : " << x.second->str();
		}
		ss << " }";
		return ss.str();
	}

	std::string JsonList::str()
	{
		bool isFirst = true;

		std::stringstream ss;
		ss << "[ ";
		for (auto& x : value)
		{
			if (!isFirst)
			{
				ss << ", ";
			}
			isFirst = false;
			ss << x->str();
		}
		ss << " ]";
		return ss.str();
	}


	JsonDocument::JsonDocument(const std::string& json) :
		input{json}
	{
		root_ = parseJson(input);
	}

	JsonDocument::~JsonDocument()
	{
		delete root_;
	}

	void expectColon(Input& input)
	{
		if (input.get() != ':')
		{
			throw std::runtime_error("While parsing json: expected ':' ; on position: " +
				std::to_string(input.getPosition()));
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

	JsonDouble* parseDouble(Input& input)
	{
		throw std::runtime_error("Implement me!");
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
			throw std::runtime_error("While parsing json: expected '\"' ; on position: " +
				std::to_string(input.getPosition()));
		}

		input.next(); // Skip quote
		std::string temp = input.readWhile(isNotQuote);
		input.next(); // Skip quote

		return new JsonString(temp);
	}

	auto parseEntry(Input& input)
	{
		JsonString* key = parseString(input);
		std::string keyString = std::move(key->value);
		delete key;

		input.skipSpace();
		expectColon(input);
		input.skipSpace();

		JsonElement* element = parseJson(input);

		return std::make_pair(std::move(keyString), element);
	}

	JsonMap* parseMap(Input& input)
	{
		// Zalozenia wejsciowe: jestesmy przy '{'
		if (input.get() != '{')
		{
			// niedobrze
			throw std::runtime_error("While parsing json: expected '{' ; on position: " +
				std::to_string(input.getPosition()));
		}
		input.next(); // Skip curly-bracket

		JsonMap* result = new JsonMap();

		input.skipSpace();
		if (input.get() == '}')
		{
			input.next();
			return result;
		}

		do
		{
			input.skipSpace();
			result->insert(parseEntry(input));
			input.skipSpace();
		} while (maybeComma(input));

		if (input.get() != '}')
		{
			throw std::runtime_error("While parsing json: expected '}' ; on position: " +
				std::to_string(input.getPosition()));
		}
		input.next(); // Skip curly-bracket

		return result;
	}


	JsonList* parseList(Input& input)
	{
		// Zalozenia wejsciowe: jestesmy przy '['
		if (input.get() != '[')
		{
			// niedobrze
			throw std::runtime_error("While parsing json: expected '[' ; on position: " +
				std::to_string(input.getPosition()));
		}
		input.next(); // Skip bracket

		JsonList* result = new JsonList();

		input.skipSpace();
		if (input.get() == ']')
		{
			input.next();
			return result;
		}

		do
		{
			input.skipSpace();
			result->push_back(parseJson(input));
			input.skipSpace();
		} while (maybeComma(input));

		if (input.get() != ']')
		{
			throw std::runtime_error("While parsing json: expected ']' ; on position: " +
				std::to_string(input.getPosition()));
		}
		input.next(); // Skip bracket

		return result;
	}

	JsonElement* parseJson(Input& input)
	{
		input.skipSpace();
		if (std::isdigit(input.get()))
		{
			return parseInteger(input);
		}
		else if (input.get() == '"')
		{
			return parseString(input);
		}
		else if (input.get() == '{')
		{
			return parseMap(input);
		}
		else if (input.get() == '[')
		{
			return parseList(input);
		}
		else
		{
			throw std::runtime_error(
				std::string("While parsing json: unexpected character '") + 
				input.get() + "' ; on position: " + std::to_string(input.getPosition()));
		}
	}

	JsonElement* parseJson(const std::string& input)
	{
		Input x(input);
		return parseJson(x);
	}
}
