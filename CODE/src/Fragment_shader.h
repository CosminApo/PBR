#ifndef _FRAGMENT_SHADER
#define _FRAGMENT_SHADER

#include "Shader.h"
class Fragment_shader :
	public Shader
{
private:

public:
	int compileShader(GLuint* _shaderID);
	Fragment_shader(const GLchar* _shader);
	Fragment_shader(const std::string* _shaderSrc);
};

#endif // !_FRAGMENT_SHADER