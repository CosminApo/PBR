#include "Object_manager.h"

Object_manager::Object_manager() : currentFragmentShader("temp"), currentVertexShader("temp"), myShaderProgram(), myTextureManager((std::string("temp")))
{
}

/**
Add an object to the list of objects that will be rendered on the screen
	@param _objectType either 2D (false) or 3D (true)
	@param _modelSrc the path to the file containing the object
	@param _fragmentShader the fragment shader associated with this object
	@param _vertexShader the vertex shader associated with this object
	@param _position the position of the object
*/
int Object_manager::addObject(bool _objectType, std::string _modelSrc, Fragment_shader _fragmentShader, Vertex_shader _vertexShader, glm::vec3 _position)
{
	//initialize the object
	Object temp = { _objectType, _modelSrc, _fragmentShader, _vertexShader, _position, "temp" }; //create new temp object from parameters passed

	//add the 5 textures to the object
	myTextureManager = *(new Texture_manager("Oni_Mask_Albedo.tga")); //load the texture
	myTextureManager.generateTexture(&temp.textureID_albedo, &programID); //generate the texture

	myTextureManager = *(new Texture_manager("Oni_Mask_Normal.tga")); //load the texture
	myTextureManager.generateTexture(&temp.textureID_normal, &programID); //generate the texture

	myTextureManager = *(new Texture_manager("Oni_Mask_Metallic.tga")); //load the texture
	myTextureManager.generateTexture(&temp.textureID_metallic, &programID); //generate the texture

	myTextureManager = *(new Texture_manager("Oni_Mask_roug.tga")); //load the texture
	myTextureManager.generateTexture(&temp.textureID_roughness, &programID); //generate the texture

	myTextureManager = *(new Texture_manager("Oni_Mask_Occlusion.tga")); //load the texture
	myTextureManager.generateTexture(&temp.textureID_ao, &programID); //generate the texture


	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_modelSrc, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return 1;
	}


	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene, &temp);
	GLuint m_vbo = 0;
	GLuint m_ebo = 0;
	glGenVertexArrays(1, &temp.vaoID);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(temp.vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, temp.vertices.size() * sizeof(Vertex), &temp.vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size() * sizeof(unsigned int),
		&temp.indices[0], GL_STATIC_DRAW);
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texUVs));
	// vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangents));
	// vertex bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangents));

	glGenBuffers(1, &m_ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, temp.indices.size() * sizeof(unsigned int),
		&temp.indices[0], GL_STATIC_DRAW);

	//add object to list
	listOfObjects.push_back(temp);
	return 0;
}


void Object_manager::setIrrMap(unsigned int _irrMap)
{
	irradianceMap = _irrMap;
}

void Object_manager::setBrdfLutTex(unsigned int _brdfLutTex)
{
	brdfLUTtex = _brdfLutTex;
}

void Object_manager::setprefilterMap(unsigned int _preFilterMap)
{
	prefilterMap = _preFilterMap;
}


/**
Processes the mesh data read by the assimp importer
	@param _mesh the mesh loaded in by the assimp importer
	@param _scene the scene loaded in by the assimp importer
	@param _obj the object where to store the data
*/
void Object_manager::processMesh(aiMesh* _mesh, const aiScene* _scene, Object* _obj)
{
	Vertex tempvec;
	// data to fill
	glm::vec3 tangent;
	glm::vec3 biTangent;
	glm::vec3 normal;

	// walk through each of the _mesh's vertices
	for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
	{
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = _mesh->mVertices[i].x;
		vector.y = _mesh->mVertices[i].y;
		vector.z = _mesh->mVertices[i].z;
		tempvec.position = vector;

		// normals
		if (_mesh->HasNormals())
		{
			vector.x = _mesh->mNormals[i].x;
			vector.y = _mesh->mNormals[i].y;
			vector.z = _mesh->mNormals[i].z;
			tempvec.normal = vector;
			_obj->normals.push_back(normal);
		}
		// texture coordinates
		if (_mesh->mTextureCoords[0]) // does the _mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = _mesh->mTextureCoords[0][i].x;
			vec.y = _mesh->mTextureCoords[0][i].y;
			tempvec.texUVs = vec;

			// tangent
			vector.x = _mesh->mTangents[i].x;
			vector.y = _mesh->mTangents[i].y;
			vector.z = _mesh->mTangents[i].z;
			tempvec.tangents = vector;

			tangent = vector;
			_obj->tangents.push_back(tangent);
			// bitangent
			vector.x = _mesh->mBitangents[i].x;
			vector.y = _mesh->mBitangents[i].y;
			vector.z = _mesh->mBitangents[i].z;
			tempvec.bitangents = vector;

			biTangent = vector;
			_obj->biTangents.push_back(biTangent);

		}
		_obj->vertices.push_back(tempvec);
	}
	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			_obj->indices.push_back(face.mIndices[j]);
	}
}

void Object_manager::setID(GLuint _id)
{
	programID = _id;
}



/**
Process the node from the scene loaded in by assimp.
	@param _node the node loaded in by the assimp importer
	@param _scene the scene loaded in by the assimp importer
	@param _obj the object where to store the data
*/
void Object_manager::processNode(aiNode* _node, const aiScene* _scene, Object* _obj)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < _node->mNumMeshes; i++)
	{
		aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
		processMesh(mesh, _scene, _obj);

	}
	// after all of the meshes, process each of the children nodes
	for (unsigned int i = 0; i < _node->mNumChildren; i++)
	{
		processNode(_node->mChildren[i], _scene, _obj);
	}
}

/**
Draw all the objects from the object list.
	@param _GL_DEPTH_TEST enables the depth testing feature of openGL (hides objects behind other objects)
	@param _GL_CULL_FACE enable the cull face feature of openGL (hides faces of objects that are not visible)
	@param _mySDLManager the render window manager currently in use
*/
int Object_manager::drawAll(bool _GL_DEPTH_TEST, bool _GL_CULL_FACE, SDL_manager* _mySDLManager)
{
	glm::mat4 offset; //generate matrix for the object offset
	bool recompile = false; //flag to check if shader program needs to be recompiled
	for (Object &x : listOfObjects) //for each object in the list
	{
		recompile = false;
		offset = glm::mat4(1.0f);
		if (x.objectType == false) //draw only the 2D objects first
		{
			/*NOTE, outdated code for 2D objects, to be fixed to work in a PBR environment*/
			if (x.fragmentShader.getShaderSrc() != currentFragmentShader.getShaderSrc()) //if the fragment shader is new
			{
				x.fragmentShader.compileShader(&fragmentShaderID); //compile the shader
				currentFragmentShader = x.fragmentShader;
				recompile = true;
			}
			if (x.vertexShader.getShaderSrc() != currentVertexShader.getShaderSrc()) //if the fragment shader is new
			{
				x.vertexShader.compileShader(&vertexShaderID); //compile the shader
				currentVertexShader = x.vertexShader;
				recompile = true;
			}
			if (recompile) //if either shader changed
			{
				myShaderProgram.compileShaderProgram(&programID, &vertexShaderID, &fragmentShaderID); //compile the program
			}
			
			offset = glm::translate(offset, x.position); //create the offset mat
			_mySDLManager->draw(&programID, &x.objectType, &x.vaoID, x, &(offset), &x.numVerts, &_GL_DEPTH_TEST, &_GL_CULL_FACE, irradianceMap, prefilterMap, brdfLUTtex); //draw the object
		}
	}
	for (Object& x : listOfObjects) //for each object in the list
	{
		recompile = false;
		offset = glm::mat4(1.0f);
		if (x.objectType == true) //draw the 3D objects after
		{
			offset = glm::translate(offset, x.position); //create the offset mat
			_mySDLManager->draw(&programID, &x.objectType, &x.vaoID, x, &(offset), &x.numVerts, &_GL_DEPTH_TEST, &_GL_CULL_FACE, irradianceMap, prefilterMap, brdfLUTtex); //draw the object
		}
	}

	return 0;
}
