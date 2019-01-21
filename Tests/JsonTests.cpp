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
	};
}