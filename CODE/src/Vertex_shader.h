#ifndef _VERTEX_SHADER_H
#define _VERTEX_SHADER_H


#include "Shader.h"
class Vertex_shader :
	public Shader
{
private:

public:
	int compileShader(GLuint* _shaderID);
	Vertex_shader(const GLchar* _shader);
	Vertex_shader(const std::string* _shaderSrc);
};

#endif // !_VERTEX_SHADER_H