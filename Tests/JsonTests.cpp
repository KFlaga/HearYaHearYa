#include "stdafx.h"
#include "CppUnitTest.h"

#include "Json.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{		
	using namespace eave;

	TEST_CLASS(JsonTests)
	{
	public:
		template<typename Func>
		void expectThrow(Func f)
		{
			try
			{
				f();
			}
			catch (...)
			{
				return;
			}
			Assert::Fail(L"Throw expected");
		}

		TEST_METHOD(ParseJsonInteger_123)
		{
			std::string text = "123";
			Input input(text);

			eave::JsonInteger* value = eave::parseInteger(input);

			Assert::AreEqual(123, value->value);
			Assert::AreEqual(3, input.getPosition());
		}

		TEST_METHOD(ParseJsonInteger_2times)
		{
			std::string text = "1234 2345";
			Input input(text);

			eave::JsonInteger* value = eave::parseInteger(input);
			Assert::AreEqual(1234, value->value);

			input.next();

			value = eave::parseInteger(input);
			Assert::AreEqual(2345, value->value);
		}

		TEST_METHOD(ParseJsonInteger_empty)
		{
			std::string text = "";
			Input input(text);
			
			expectThrow([&]()
			{
				eave::JsonInteger* value = eave::parseInteger(input);
			});
		}

		TEST_METHOD(ParseJsonInteger_nonDigit)
		{
			std::string text = "aaa";
			Input input(text);

			expectThrow([&]()
			{
				eave::JsonInteger* value = eave::parseInteger(input);
			});
		}

		TEST_METHOD(ParseJsonString_123)
		{
			std::string text = "\"123\"";
			Input input(text);

			eave::JsonString* value = eave::parseString(input);

			std::string expected = "123";
			Assert::AreEqual(expected, value->value);
		}

		TEST_METHOD(ParseJsonString_empty)
		{
			std::string text = "\"\"";
			Input input(text);

			eave::JsonString* value = eave::parseString(input);

			std::string expected = "";
			Assert::AreEqual(expected, value->value);
		}

		TEST_METHOD(ParseJsonString_noEntryQuote)
		{
			std::string text = "123\"";
			Input input(text);

			expectThrow([&]()
			{
				eave::JsonString* value = eave::parseString(input);
			});
		}

		TEST_METHOD(ParseJsonString_noTerminalQuote)
		{
			std::string text = "\"123";
			Input input(text);
			
			expectThrow([&]()
			{
				eave::JsonString* value = eave::parseString(input);
			});
		}

		TEST_METHOD(ParseJsonMap_empty)
		{
			std::string text = "{ }";
			Input input(text);

			eave::JsonMap* value = eave::parseMap(input);

			Assert::IsTrue(0 == value->size());
		}

		TEST_METHOD(ParseJsonMap_oneElement)
		{
			std::string text = "{ \"x\" : 123 }";
			Input input(text);

			eave::JsonMap* value = eave::parseMap(input);

			Assert::IsTrue(1 == value->size());
			Assert::IsTrue(value->contains("x"));
			Assert::IsTrue((*value)["x"].asInteger() == 123);
		}

		TEST_METHOD(ParseJsonMap_twoElements)
		{
			std::string text = "{ \"x\" : 123, \"y\" : \"abc\" }";
			Input input(text);

			eave::JsonMap* value = eave::parseMap(input);

			Assert::IsTrue(2 == value->size());

			Assert::IsTrue(value->contains("x"));
			Assert::IsTrue((*value)["x"].asInteger() == 123);

			Assert::IsTrue(value->contains("y"));
			Assert::IsTrue((*value)["y"].asString().value == "abc");
		}

		TEST_METHOD(ParseJsonList_empty)
		{
			std::string text = "[]";
			Input input(text);

			eave::JsonList* value = eave::parseList(input);

			Assert::IsTrue(0 == value->size());
		}

		TEST_METHOD(ParseJsonList_oneElement)
		{
			std::string text = "[123]";
			Input input(text);

			eave::JsonList* value = eave::parseList(input);

			Assert::IsTrue(1 == value->size());

			Assert::IsTrue((*value)[0].asInteger() == 123);
		}

		TEST_METHOD(ParseJsonList_twoElements)
		{
			std::string text = "[\"aaa\", 345]";
			Input input(text);

			eave::JsonList* value = eave::parseList(input);

			Assert::IsTrue(2 == value->size());

			Assert::IsTrue((*value)[0].asString().value == "aaa");
			Assert::IsTrue((*value)[1].asInteger() == 345);
		}
		
		TEST_METHOD(JsonInteger_toString)
		{
			// Check if "str()" works ok by first using it and then parse back to JsonInteger
			// As we are sure parsing is ok, it should convert it back to source value

			JsonInteger value{ 123 };

			std::string repr = value.str();

			Input input(repr);
			eave::JsonInteger* result = eave::parseInteger(input);

			Assert::IsTrue(value.value == result->value);
		}

		TEST_METHOD(JsonString_toString)
		{
			// Check if "str()" works ok by first using it and then parse back to JsonInteger
			// As we are sure parsing is ok, it should convert it back to source value

			JsonString value{ "Abc" };

			std::string repr = value.str();

			Input input(repr);
			eave::JsonString* result = eave::parseString(input);

			Assert::IsTrue(value.value == result->value);
		}

		TEST_METHOD(JsonMap_toString)
		{
			JsonMap value;

			value.insert({ "key1", new JsonInteger(123) });
			value.insert({ "key2", new JsonString("abc") });

			std::string repr = value.str();

			Input input(repr);
			eave::JsonMap* result = eave::parseMap(input);

			Assert::IsTrue(result->contains("key1"));
			Assert::IsTrue((*result)["key1"].asInteger() == 123);
			Assert::IsTrue(result->contains("key2"));
			Assert::IsTrue((*result)["key2"].asString().value == "abc");
		}

		TEST_METHOD(JsonList_toString)
		{
			JsonList value;

			value.push_back( new JsonInteger(123) );
			value.push_back( new JsonString("abc") );

			std::string repr = value.str();

			Input input(repr);
			eave::JsonList* result = eave::parseList(input);

			Assert::IsTrue((*result)[0].asInteger() == 123);
			Assert::IsTrue((*result)[1].asString().value == "abc");
		}


		TEST_METHOD(ParseJsonDouble_parserNumber)
		{
			std::string text = "123.0";
			Input input(text);

			eave::JsonDouble* value = eave::parseDouble(input);

			Assert::AreEqual(123.0, value->value);
			Assert::AreEqual(5, input.getPosition());
		}

		TEST_METHOD(ParseJsonDouble_shiftsInputAfterNumber)
		{
			std::string text = "34.23";
			Input input(text);

			eave::JsonDouble* value = eave::parseDouble(input);

			Assert::AreEqual(5, input.getPosition());
		}

		TEST_METHOD(ParseJsonDouble_inMap)
		{
			std::string text = "{ \"x\" : 54.23 }";
			Input input(text);

			eave::JsonMap* value = eave::parseMap(input);

			Assert::IsTrue(1 == value->size());
			Assert::IsTrue(value->contains("x"));
			Assert::IsTrue((*value)["x"].asDouble().value == 54.23);
		}

		TEST_METHOD(JsonDouble_toString)
		{
			// Check if "str()" works ok by first using it and then parse back to JsonInteger
			// As we are sure parsing is ok, it should convert it back to source value

			JsonDouble value{ 23.12 };

			std::string repr = value.str();

			Input input(repr);
			eave::JsonDouble* result = eave::parseDouble(input);

			Assert::IsTrue(value.value == result->value);
		}

		TEST_METHOD(ParseJsonInteger_negative)
		{
			std::string text = "-123";
			Input input(text);

			eave::JsonInteger* value = eave::parseInteger(input);

			Assert::AreEqual(-123, value->value);
		}

		TEST_METHOD(ParseJsonDouble_negative)
		{
			std::string text = "-123.0";
			Input input(text);

			eave::JsonDouble* value = eave::parseDouble(input);

			Assert::AreEqual(-123.0, value->value);
		}
	};
}