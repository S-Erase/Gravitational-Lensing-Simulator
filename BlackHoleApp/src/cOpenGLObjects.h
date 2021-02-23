#pragma once
#include "cPCH.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <unordered_map>

using sampler2D = int;
using samplerCube = int;

class cOpenGLVBO {
public:
	~cOpenGLVBO() { glDeleteBuffers(1, &m_BufferID); }
	inline void Bind() { glBindBuffer(GL_ARRAY_BUFFER, m_BufferID); }

	inline size_t GetSize() { return m_Size; }
protected:
	GLuint m_BufferID = 0;
	size_t m_Size = 0;
};

class cOpenGLDynamicVBO : public cOpenGLVBO
{
public:
	cOpenGLDynamicVBO(size_t size);
	void InsertData(size_t size, const void* data);

	inline size_t GetCapacity() { return m_Capacity; }
private:
	size_t m_Capacity = 0;
};

class cOpenGLStaticVBO : public cOpenGLVBO
{
public:
	cOpenGLStaticVBO(size_t size, const void* data);
private:
};

class cOpenGLShader
{
public:
	cOpenGLShader(const std::string& filepath);
	~cOpenGLShader() { glDeleteProgram(m_ShaderID); }
	void Use() const { glUseProgram(m_ShaderID); }

	void SetUniformMat4(const char* u_name, const glm::mat4& mat) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		//SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(mat));

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformSampler2D(const char* u_name, sampler2D val) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform1i(uniformID, val);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformSamplerCube(const char* u_name, samplerCube val) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform1i(uniformID, val);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformInt(const char* u_name, int i0) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform1i(uniformID, i0);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformFloat(const char* u_name, float v0) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform1f(uniformID, v0);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformFloat2(const char* u_name, float v0, float v1) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform2f(uniformID, v0, v1);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
	void SetUniformFloat3(const char* u_name, const glm::vec3& vec) {
		glUseProgram(m_ShaderID);
		GLint uniformID = glGetUniformLocation(m_ShaderID, u_name);
		SE_ASSERT(uniformID != -1, "Uniform '" + std::string(u_name) + "' could not be found!");
		glUniform3fv(uniformID, 1, &vec.x);

		if (m_UniformCache.find(u_name) == m_UniformCache.end()) {
			m_UniformCache[u_name] = uniformID;
		}
	}
private:
	GLuint m_ShaderID = 0;
	std::unordered_map<std::string, GLuint> m_UniformCache;
};

class cOpenGLStaticIBO
{
public:
	cOpenGLStaticIBO(size_t size, const void* data, GLenum type);
	~cOpenGLStaticIBO() { glDeleteBuffers(1, &m_BufferID); }
	inline void Bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}
	inline GLenum GetType() { return m_Type; }
	inline size_t GetCount() { return m_Count; }
private:
	GLuint m_BufferID = 0;
	size_t m_Count = 0;
	GLenum m_Type = 0;
};

class cOpenGLVAO
{
private:
	struct cOpenGLVertexAttribute
	{
		GLint size;
		GLenum type;
	};
public:
	cOpenGLVAO();
	~cOpenGLVAO();
	void SetLayout(std::initializer_list<cOpenGLVertexAttribute> layout);
	inline void Bind() { glBindVertexArray(m_ArrayID); }
	void BindVertexBuffer(const std::shared_ptr<cOpenGLVBO>& vbo);
	void DrawElements(std::shared_ptr<cOpenGLStaticIBO>& ibo, GLenum mode, size_t count);
	void DrawArrays(GLenum mode);
private:
	GLuint m_ArrayID = 0;
	size_t m_Stride = 0;
	std::shared_ptr<cOpenGLVBO> m_vboRef;
};

class cOpenGLTexture {
public:
	cOpenGLTexture(const char* filepath);
	void Bind(sampler2D slot = 0) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}
private:
	GLuint m_TextureID = 0;
	int m_Width = 0, m_Height = 0, m_Channels = 0;
};

class cOpenGLCubemap {
public:
	cOpenGLCubemap(const char* frontfilepath, const char* backfilepath, const char* rightfilepath, 
		const char* leftfilepath, const char* upfilepath, const char* downfilepath);
	void Bind() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
	}
	void SetTexture(GLenum target, const char* filepath);
	void UseNet(const char* filepath);
private:
	GLuint m_TextureID = 0;
	int m_Size = 1280;
};