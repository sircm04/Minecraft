#pragma once

struct ShaderEntry
{
	unsigned int m_Type;
	std::string m_Source;
};

class Shader
{
private:
	std::vector<ShaderEntry> m_Shaders;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::vector<ShaderEntry>& shaders);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetVec2(const std::string& name, glm::vec2 value);
	void SetVec3(const std::string& name, glm::vec3 value);
	void SetVec4(const std::string& name, glm::vec4 value);
	void SetMat4(const std::string& name, glm::mat4 value);

private:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	const std::string GetShaderType(unsigned int id);

	int GetUniformLocation(const std::string& name);
};