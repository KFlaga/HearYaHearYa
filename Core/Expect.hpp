#pragma once

#include <stdexcept>

// To disable Expect checks define EAVE_DISABLE_EXPECT_CHECKS before this header
//#ifdef NDEBUG
//#define EAVE_DISABLE_EXPECT_CHECKS
//#endif

namespace eave
{
	struct ExpectationNoMet : std::runtime_error
	{
		using runtime_error::runtime_error;
	};

#ifndef EAVE_DISABLE_EXPECT_CHECKS
	inline void Expect(bool cond, const char* msg = "Expectation not met")
	{
		if (!cond)
		{
			throw ExpectationNoMet(msg);
		}
	}

	inline void Expect(bool cond, std::string(*msg)())
	{
		if (!cond)
		{
			throw ExpectationNoMet(msg());
		}
	}
#else
	inline void Expect(bool cond, const char* msg = "")
	{
	}

	inline void Expect(bool cond, std::string(*msg)())
	{
	}
#endif
}

#define EAVE_STRINGIFY_IMPL(x) #x
#define EAVE_STRINGIFY(x) EAVE_STRINGIFY_IMPL(x)

#ifndef EAVE_DISABLE_EXPECT_CHECKS

#define EAVE_EXPECT(cond) \
	eave::Expect((cond), []() { \
		return "In " + \
               std::string(__FILE__) + ": " + std::string(EAVE_STRINGIFY(__LINE__)) + \
               std::string(" - expectation no met: ") + EAVE_STRINGIFY(cond); \
	})

#else

#define EAVE_EXPECT(cond) eave::Expect((cond))

#endif
