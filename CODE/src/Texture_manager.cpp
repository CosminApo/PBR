#include "Texture_manager.h"

// Needs to be defined before the include in exactly one comp unit
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
The texture manager will load and bind a texture to the shader program
	@param _data the path of the texture file
*/
Texture_manager::Texture_manager(std::string _data)
{
	if (_data != "temp")
	{
		//load in the data
		data = stbi_load((_data).c_str(), &width, &height, &nrComponents, 0);

		if (data == NULL)
		{
			std::cout << "texture no load: " << _data;
		}

		//set the components format
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
	}
}

/**
Generate the texture and receive the texture ID.
	@param _textureID the ID of the  texture.
	@param _programID the ID of the program that will use the texture
*/
int Texture_manager::generateTexture(GLuint* _textureID, GLuint* _programID)
{
	//create and bind a texture
	*_textureID = 0;
	glGenTextures(1, &*_textureID);
	if (!*_textureID)
	{
		std::cout << "Texture could not be created" << std::endl;
		throw std::exception();
	}

	////glActiveTexture(GL_TEXTURE0 + 1); multitexture
	glBindTexture(GL_TEXTURE_2D, *_textureID);

	//upload the image data to the bound texture unit in the gpu
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	//free the loaded data cause its now on the gpu
	free(data);

	//generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);


	return 0;
}

/**
Generate the texture and receive the texture ID.
	@param _textureID the ID of the  texture.
	@param _programID the ID of the program that will use the texture
	@param _path the path of the texture file
*/
int Texture_manager::generateHDR(GLuint* _textureID, GLuint* _programID, std::string _path)
{
	//flip texture
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(_path.c_str(), &width, &height, &nrComponents, 0);
	unsigned int hdrTexture = 0;

	//genereate texture 
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//free data
	stbi_image_free(data);

	//store the id
	*_textureID = hdrTexture;

	//reset the parameter
	stbi_set_flip_vertically_on_load(false);
	return 0;
}
