#include "stdafx.h"
#include "CppUnitTest.h"

#include "Json_2.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{		
	using namespace eave;

	TEST_CLASS(JsonTests)
	{
	public:
		TEST_METHOD(ParseJsonInteger_123)
		{
			Input input("123");

			eave::JsonInteger* value = eave::parseInteger(input);

			Assert::AreEqual(123, value->value);
		}

		TEST_METHOD(ParseJsonInteger_2times)
		{
			Input input("1234 2345");

			eave::JsonInteger* value = eave::parseInteger(input);

			Assert::AreEqual(1234, value->value);

			input.next();

			Assert::AreEqual(2345, value->value);
		}

		TEST_METHOD(ParseJsonInteger_empty)
		{
			Input input("");
			
			bool thrown = false;
			try
			{
				eave::JsonInteger* value = eave::parseInteger(input);
			}
			catch (...)
			{
				thrown = true;
			}
			Assert::IsTrue(thrown);
		}

		TEST_METHOD(ParseJsonInteger_nonDigit)
		{
			Input input("aaa");

			bool thrown = false;
			try
			{
				eave::JsonInteger* value = eave::parseInteger(input);
			}
			catch (...)
			{
				thrown = true;
			}
			Assert::IsTrue(thrown);
		}

		TEST_METHOD(ParseJsonString_123)
		{
			Input input("\"123\"");

			eave::JsonString* value = eave::parseString(input);

			std::string expected = "123";
			Assert::AreEqual(expected, value->value);
		}

		TEST_METHOD(ParseJsonString_empty)
		{
			Input input("\"\"");

			eave::JsonString* value = eave::parseString(input);

			std::string expected = "";
			Assert::AreEqual(expected, value->value);
		}

		TEST_METHOD(ParseJsonString_noEntryQuote)
		{
			Input input("123\"");

			bool thrown = false;
			try
			{
				eave::JsonString* value = eave::parseString(input);
			}
			catch (...)
			{
				thrown = true;
			}
			Assert::IsTrue(thrown);
		}

		TEST_METHOD(ParseJsonString_noTerminalQuote)
		{
			Input input("\"123");

			bool thrown = false;
			try
			{
				eave::JsonString* value = eave::parseString(input);
			}
			catch (...)
			{
				thrown = true;
			}
			Assert::IsTrue(thrown);
		}

		TEST_METHOD(ParseJsonMap_empty)
		{
			Input input("{ }");

			eave::JsonMap* value = eave::parseMap(input);

		}

		TEST_METHOD(ParseJsonMap_oneElement)
		{
			Input input("{ \"x\" : 123 }");

			eave::JsonMap* value = eave::parseMap(input);

			Assert::IsTrue(1 == value->value.size());
			Assert::IsTrue(value->value.find("x") != value->value.end());
			Assert::IsTrue(value->value["x"]->get<JsonInteger>().value == 123);
		}
	};
}