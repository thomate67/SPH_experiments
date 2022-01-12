#pragma once

#include <src/libraries/Utility/definitions.h>
#include <src/libraries/Utility/fileReader.h>

class ShaderProgram
{
public:
	ShaderProgram(const char* vertexpath, const char* fragmentpath);
	ShaderProgram(const char * vertexpath, const char * fragmentpath, const char * geometrypath);
	ShaderProgram(const char* computepath);
	~ShaderProgram();

	void use();
	GLuint getProgram();
	void reload(int type);
	void updateUniform(const GLchar* name, glm::mat4 m);
	void updateUniform(const GLchar* name, glm::vec4 v);
	void updateUniform(const GLchar* name, glm::vec3 v);
	void updateUniform(const GLchar* name, glm::vec2 v);
	void updateUniform(const GLchar* name, float f);
	void updateUniform(const GLchar* name, int i);
	void updateUniform(const GLchar* name, unsigned int ui);

private:
	GLuint createShader(const char* path, GLenum type);
	void loadFromSource(const char* path, GLuint shaderID);

	GLint findUniform(const GLchar * name);

	void checkShaderStatus(GLuint shaderID);

	void checkProgramStatus(GLuint programID);

	GLuint m_program;
	const char* m_vertexPath;
	const char* m_fragmentPath;
	const char* m_geometryPath;
};