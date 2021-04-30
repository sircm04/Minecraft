#include "../pch.h"
#include "Shader.h"

Shader::Shader(const std::unordered_map<unsigned int, std::string>& shaders)
	: m_Shaders(shaders)
{
	m_RendererID = glCreateProgram();

	for (auto shader : shaders)
	{
		unsigned int id = CompileShader(shader.first, shader.second);
		glAttachShader(m_RendererID, id);
		glDeleteShader(id);
	}

	glLinkProgram(m_RendererID);
	glValidateProgram(m_RendererID);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << GetShaderType(type) << " shader!" << std::endl;
		std::cout << message << std::endl;
		delete[] message;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

const std::string Shader::GetShaderType(unsigned int id)
{
	switch (id)
	{
		case GL_VERTEX_SHADER:		  return "vertex";
		case GL_FRAGMENT_SHADER:	  return "fragment";
		case GL_TESS_CONTROL_SHADER:  return "tessellation";
		case GL_GEOMETRY_SHADER:	  return "geometry";
		case GL_COMPUTE_SHADER:		  return "compute";
	}

	return "unknown";
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

int Shader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform '" << name.c_str() << "' doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, glm::vec2 value) const
{
	glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, glm::vec3 value) const
{
	glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, glm::vec4 value) const
{
	glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
