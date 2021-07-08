#include "Fragment_shader.h"

/**
Compile the shader and receive the shader ID.
	@param _ShaderID the ID of the  shader.
*/
int Fragment_shader::compileShader(GLuint* _shaderID)
{
	GLint success = 0;
    *_shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(*_shaderID, 1, &(Shader::shader), NULL);
	glCompileShader(*_shaderID);
	glGetShaderiv(*_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cout << "Error with fragment shader compiling\n";
		GLint maxLength = 0;
		glGetShaderiv(*_shaderID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(*_shaderID, maxLength, &maxLength, &errorLog[0]);
		std::cout << &errorLog.at(0) << std::endl;
		throw std::exception();
	}
	return 0;
}

Fragment_shader::Fragment_shader(const GLchar* _shader) : Shader{ _shader }
{
}

Fragment_shader::Fragment_shader(const std::string* _shaderSrc) : Shader{_shaderSrc}
{
}
