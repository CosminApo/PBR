#include "Vertex_shader.h"

/**
Compile the shader and receive the shader ID.
	@param _ShaderID the ID of the  shader.
*/
int Vertex_shader::compileShader(GLuint* _shaderID)
{
	*_shaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(*_shaderID, 1, &(Shader::shader), NULL); //null = null terminated strings
	glCompileShader(*_shaderID);
	GLint success = 0;
	glGetShaderiv(*_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cout << "Error with vertex shader compiling\n";
		GLint maxLength = 0;
		glGetShaderiv(*_shaderID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(*_shaderID, maxLength, &maxLength, &errorLog[0]);
		std::cout << &errorLog.at(0) << std::endl;
		throw std::exception();
	}
	return 0;
}

Vertex_shader::Vertex_shader(const GLchar* _shader) : Shader{_shader}
{
}

Vertex_shader::Vertex_shader(const std::string* _shaderSrc) : Shader{_shaderSrc}
{
}
