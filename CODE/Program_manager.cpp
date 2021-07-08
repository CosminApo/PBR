#include "Program_manager.h"
/**
Compile the program and receive the program ID.
	@param _programID ID of the  program
	@param _vertexShaderID ID of the vertex shader
	@param _fragmentShaderID ID of the fragment shader
*/
int Program_manager::compileShaderProgram(GLuint* _programID, GLuint* _vertexShaderID, GLuint* _fragmentShaderID)
{
	GLint success = 0;
	*_programID = glCreateProgram();
	glAttachShader(*_programID, *_vertexShaderID);
	glAttachShader(*_programID, *_fragmentShaderID);

	glGetShaderiv(*_vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetShaderiv(*_fragmentShaderID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(*_fragmentShaderID, maxLength, &maxLength, &errorLog[0]);
		std::cout << &errorLog.at(0) << std::endl;
		throw std::exception();
	}

	//cleanup
	glDetachShader(*_programID, *_vertexShaderID);
	glDeleteShader(*_vertexShaderID);
	glDetachShader(*_programID, *_fragmentShaderID);
	glDeleteShader(*_fragmentShaderID);
	return 0;
}
