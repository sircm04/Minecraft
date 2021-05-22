#pragma once

namespace Utils
{
	static const std::string ReadFile(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::string string, line;

		while (std::getline(stream, line))
			string += line + "\n";
		stream.close();

		return string;
	}

	template <typename T>
	static T ApproachValue(T x, T value, T i)
	{
		if (x < value)
		{
			T numSteps = x + i;

			if (numSteps > value)
				return value;
			else
				return numSteps;
		}

		if (x > value)
		{
			T numSteps = x - i;

			if (numSteps < value)
				return value;
			else
				return numSteps;
		}
	}
}
