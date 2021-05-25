#pragma once

class Shader
{
private:
	std::unordered_map<unsigned int, std::string> m_Shaders;
	unsigned int m_RendererID;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::unordered_map<unsigned int, std::string>& shaders);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, glm::vec2 value) const;
	void SetVec3(const std::string& name, glm::vec3 value) const;
	void SetVec4(const std::string& name, glm::vec4 value) const;
	void SetMat4(const std::string& name, glm::mat4 value) const;

private:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	const std::string GetShaderType(unsigned int id);

	unsigned int GetUniformLocation(const std::string& name) const;
};
