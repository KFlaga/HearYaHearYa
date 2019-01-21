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
		String,
		Map,
		List
	};

	struct JsonInteger;
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

		std::string str() override
		{
			return std::to_string(value);
		}

		operator int()
		{
			return value;
		}

		int value;
	};

	struct JsonString : JsonElement
	{
		JsonString(const std::string& x) :
			JsonElement{ JsonElementType::String },
			value{ x }
		{}

		std::string str() override
		{
			return value;
		}

		int size()
		{
			return int(value.size());
		}

		operator std::string&()
		{
			return value;
		}

		std::string value; // todo: string_view
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

		std::string str() override;

		JsonElement& operator[](const std::string& key) override
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
		std::map<std::string, JsonElement*> value; // todo: unique_ptr on JsonElement, string_view on key
	};

	struct JsonList : JsonElement
	{
		JsonList() :
			JsonElement{ JsonElementType::List }
		{}

		std::string str() override
		{
			return ""; // TODO
		}

		JsonElement& operator[](int index) override
		{
			return *value.at(index);
		}

		std::vector<JsonElement*> value; // todo: unique_ptr of JsonElement
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
	JsonString* parseString(Input& input);
	JsonMap* parseMap(Input& input);
	JsonList* parseList(Input& input);
	JsonElement* parseJson(Input& input);

	JsonElement* parseJson(const std::string& input);
}
