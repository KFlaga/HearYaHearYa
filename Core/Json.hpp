#pragma once

#include "Input.hpp"

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace eave
{
	enum class JsonElementType
	{
		Integer,
		Double,
		String,
		Map,
		List
	};

	struct JsonInteger;
	struct JsonDouble;
	struct JsonString;
	struct JsonMap;
	struct JsonList;

	struct JsonElement
	{
		virtual std::string str() = 0;

		virtual JsonElement& operator[](int)
		{
			throw std::runtime_error("operator[](int) not supported");
		}

		virtual JsonElement& operator[](const std::string&)
		{
			throw std::runtime_error("operator[](const std::string&) not supported");
		}

		JsonElementType type()
		{
			return type_;
		}

		JsonInteger& asInteger();
		JsonString& asString();
		JsonMap& asMap();
		JsonList& asList();
		JsonDouble& asDouble();
		
	protected:
		JsonElement(JsonElementType x) : type_{ x } {}

		void expect(JsonElementType x);

		JsonElementType type_;
	};

	struct JsonInteger : JsonElement
	{
		JsonInteger(int x) : 
			JsonElement{ JsonElementType::Integer },
			value { x }
		{}

		std::string str()
		{
			return std::to_string(value);
		}

		operator int()
		{
			// converts JsonInteger to int, so its possible to write:
			// JsonInteger x(1);
			// int y = x;
			return value;
		}

		int value;
	};

	struct JsonDouble : JsonElement
	{
		JsonDouble(double x) :
			JsonElement{ JsonElementType::Double },
			value{ x }
		{}

		std::string str()
		{
			throw std::runtime_error("Implement me!");
		}

		double value;
	};

	struct JsonString : JsonElement
	{
		JsonString(const std::string& x) :
			JsonElement{ JsonElementType::String },
			value{ x }
		{}

		std::string str()
		{
			return "\"" + value + "\"";
		}

		int size()
		{
			return int(value.size());
		}

		operator std::string&()
		{
			return value;
		}

		std::string value;
	};

	struct JsonMap : JsonElement
	{
		JsonMap() :
			JsonElement{ JsonElementType::Map }
		{}

		~JsonMap()
		{
			for (auto item : value)
			{
				delete item.second;
			}
		}

		std::string str();

		JsonElement& operator[](const std::string& key)
		{
			return *value.at(key);
		}

		int size()
		{
			return int(value.size());
		}

		auto begin()
		{
			return value.begin();
		}

		auto end()
		{
			return value.end();
		}

		auto find(const std::string& key)
		{
			return value.find(key);
		}

		auto contains(const std::string& key)
		{
			return find(key) != end();
		}

		void insert(const std::pair<std::string, JsonElement*>& entry)
		{
			value.insert(entry);
		}

	private:
		std::map<std::string, JsonElement*> value;
	};

	struct JsonList : JsonElement
	{
		JsonList() :
			JsonElement{ JsonElementType::List }
		{}

		~JsonList()
		{
			for (auto item : value)
			{
				delete item;
			}
		}

		std::string str();

		JsonElement& operator[](int index)
		{
			return *value.at(index);
		}

		int size()
		{
			return int(value.size());
		}

		auto begin()
		{
			return value.begin();
		}

		auto end()
		{
			return value.end();
		}

		void push_back(JsonElement* element)
		{
			value.push_back(element);
		}

	private:
		std::vector<JsonElement*> value;
	};

	struct JsonDocument
	{
		JsonDocument(const std::string& json);
		~JsonDocument();

		JsonElement& root() const { return *root_; }

	private:
		std::string input;
		JsonElement* root_;
	};

	JsonInteger* parseInteger(Input& input);
	JsonDouble* parseDouble(Input& input);
	JsonString* parseString(Input& input);
	JsonMap* parseMap(Input& input);
	JsonList* parseList(Input& input);
	JsonElement* parseJson(Input& input);

	JsonElement* parseJson(const std::string& input);
}
