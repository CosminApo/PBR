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

	
	//perform the link and check for failure

	glLinkProgram(*_programID);
	glGetProgramiv(*_programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetProgramiv(*_programID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(*_programID, maxLength, &maxLength, &errorLog[0]);
		std::cout << &errorLog.at(0) << std::endl;
		std::cout << "Error with linking";
		throw std::exception();
	}



	//cleanup
	glDetachShader(*_programID, *_vertexShaderID);
	glDeleteShader(*_vertexShaderID);
	glDetachShader(*_programID, *_fragmentShaderID);
	glDeleteShader(*_fragmentShaderID);
	return 0;
}
