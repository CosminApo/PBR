
#include <iostream> /*error logging*/
#include <string> /*file paths and error logging*/
#include "Vertex_shader.h"
#include "Fragment_shader.h"
#include <list> /*store list of objects*/
#include "glm/glm.hpp"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texUVs;
	glm::vec3 tangents;
	glm::vec3 bitangents;
};

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

	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> biTangents;
	std::vector<glm::vec3> normals;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};