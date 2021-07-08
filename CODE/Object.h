#pragma once
#include <iostream> /*error logging*/
#include <string> /*file paths and error logging*/
#include "src/"
#include "Fragment_shader.h"
#include "Program_manager.h"
#include <list> /*store list of objects*/
#include "bugl.h" /*model loading*/
#include "Texture_manager.h" /*texture loading*/
#include "SDL_manager.h" /*window manager for windows*/

struct Object
{
	bool objectType; //true = 3d false = 2d
	std::string modelSrc; //path to file
	Fragment_shader fragmentShader;
	Vertex_shader vertexShader;
	glm::vec3 position;
	std::string texturePath;
	GLuint vaoID = 0;
	size_t numVerts = 0;
	GLuint textureID_albedo;
	GLuint textureID_normal;
	GLuint textureID_metallic;
	GLuint textureID_roughness;
	GLuint textureID_ao;
};