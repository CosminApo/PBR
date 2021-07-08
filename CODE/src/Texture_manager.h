#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H

#include <string>
#include <GL/glew.h> /*Open GL drawing operations*/
#include <iostream> /*Console error logging*/
#include <string>


class Texture_manager
{
private:
	unsigned char* data;
	int width, height, nrComponents;
	GLenum format;
public:
	Texture_manager(std::string _data);
	int generateTexture(GLuint* _textureID, GLuint* _programID);
	int generateHDR(GLuint* _textureID, GLuint* _programID, std::string _path);
};

#endif // !_TEXTURE_MANAGER_H
