#ifndef _SHADER_H
#define _SHADER_H

#include <iostream> /*Console error logging*/
#include <vector> /*Console error logging*/
#include <GL/glew.h> /*Open GL drawing operations*/
#include <string>
#include <fstream> /*File reading*/

class Shader
{
private:
protected:
	const GLchar* shader;
	std::string shaderSrc;
public:
	Shader(const std::string* _shaderSrc);
	Shader(const GLchar* _shader);
	virtual int compileShader(GLuint* _shaderID);

	std::string getShaderSrc() { return shaderSrc; };
};

#endif

