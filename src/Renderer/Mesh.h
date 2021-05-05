#pragma once

#include "../pch.h"

template<typename T = float>
struct Mesh
{
	std::vector<T> vertices;
	std::vector<unsigned int> indices;
};
