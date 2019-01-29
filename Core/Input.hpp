#pragma once

#include <string>
#include <stdexcept>
#include <cctype>


namespace eave
{
	class Input
	{
	public:
		Input(const std::string& x) : input{ x } {}

		bool hasNext()
		{
			return pos < int(input.size());
		}

		void next()
		{
			if (!hasNext())
			{
				throw std::runtime_error("End of input");
			}
			pos++;
		}

		char get()
		{
			return input[pos];
		}

		void reset()
		{
			pos = -1;
		}

		int getPosition()
		{
			return pos;
		}

		template<typename Func>
		std::string readWhile(Func condition)
		{
			// Reads next characters while given condition is true
			std::string result;
			while (condition(get())) // todo: find start and end position and use substr
			{
				result.push_back(get());

				if (!hasNext())
				{
					break;
				}
				next();
			}
			return result;
		}

		void skipSpace()
		{
			while (std::isspace(get()))
			{
				if (hasNext())
					next();
				else
					break;
			}
		}

	private:
		int pos = 0;
		const std::string& input; // todo: string_view or consider input iterator (needs template)
	};
}