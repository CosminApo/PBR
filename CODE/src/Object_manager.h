#ifndef _OBJECT_MANAGER_H
#define _OBJECT_MANAGER_H

#include <iostream> /*error logging*/
#include <string> /*file paths and error logging*/
#include "Vertex_shader.h"
#include "Fragment_shader.h"
#include "Program_manager.h"
#include <list> /*store list of objects*/
#include "bugl.h" /*model loading*/
#include "Texture_manager.h" /*texture loading*/
#include "SDL_manager.h" /*window manager for windows*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Object_manager
{


private:
	Fragment_shader currentFragmentShader;
	Vertex_shader currentVertexShader;
	GLuint vertexShaderID = 0;
	GLuint fragmentShaderID = 0;
	
	GLuint programID = 0;
	Texture_manager myTextureManager;
	std::list<Object> listOfObjects;
	unsigned int irradianceMap;
	unsigned int prefilterMap;
	unsigned int brdfLUTtex;

public:
	Program_manager myShaderProgram;
	Object_manager();
	int addObject(bool _objectType, std::string _modelSrc, Fragment_shader _fragmentShader, Vertex_shader _vertexShader, glm::vec3 _position);
	int drawAll(bool _GL_DEPTH_TEST, bool _GL_CULL_FACE, SDL_manager* _mySDLManager);
	void processNode(aiNode* _node, const aiScene* _scene, Object* _obj);
	void processMesh(aiMesh* _mesh, const aiScene* _scene, Object* obj);
	void setID(GLuint _id);
	void setIrrMap(unsigned int _irrMap);
	void setBrdfLutTex(unsigned int _brdfLutTex);
	void setprefilterMap(unsigned int _preFilterMap);

};

#endif // !_OBJECT_MANAGER_H