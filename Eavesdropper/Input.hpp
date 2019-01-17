#pragma once

#include <string>
#include <stdexcept>


namespace eave
{
	class Input
	{
	public:
		Input(const std::string& x) : input{ x } {}

		bool hasNext()
		{
			return pos < int(input.size()) - 1;
		}

		char next()
		{
			return pos++;
		}

		char get()
		{
			return input[pos];
		}

		void reset()
		{
			pos = -1;
		}

		template<typename Func>
		std::string readWhile(Func condition)
		{
			// Reads next characters while given condition is true
			std::string result;
			while (condition(get()))
			{
				result.push_back(get());

				if (! hasNext())
				{
					throw std::runtime_error("Unexpected end od input");
				}
				next();
			}
			return result;
		}

	private:
		int pos = 0;
		const std::string& input;
	};
}