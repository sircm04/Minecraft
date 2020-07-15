#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace Utils
{
	static std::string ReadFile(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::string string, line;

		while (std::getline(stream, line))
			string += line + "\n";
		stream.close();

		return string;
	}
}