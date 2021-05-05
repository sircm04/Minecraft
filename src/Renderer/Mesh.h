#pragma once

#include "../pch.h"

template<typename T>
struct Mesh
{
	std::vector<T> vertices;
	std::vector<unsigned int> indices;
};
