#include "Shader.h"

/**
Create a shader from a file.
	@param _shaderSrc path of the file containing the shader source code
*/
Shader::Shader(const std::string* _shaderSrc)
{
	//initialize vars
	shader = 0;
	shaderSrc = "";

	//open file
	std::ifstream inFile;
	inFile.open(*_shaderSrc);
	if (!inFile) //check if file was opened
	{
		std::cerr << "Could not open the file: " << _shaderSrc;
		throw std::exception();
	}
	
	//read file line by line
	std::string line = "";
	char x = 0;
	while (std::getline(inFile, line))
	{
		shaderSrc += line;
		shaderSrc.push_back('\n'); //add a new line char at the end of each line
	}
	shader = shaderSrc.c_str(); //copy contents into shader var
	inFile.close(); //close the file
}

/**
Create a shader by passing the source code of the shader.
	@param _vertexShader source code of the shader
*/
Shader::Shader(const GLchar* _vertexShader)
{
	shader = _vertexShader;
}


/**
Compile the shader and receive the shader ID.
	@param _ShaderID the ID of the  shader. 
*/
int Shader::compileShader(GLuint* _vertexShaderID)
{
	return 0;
}
