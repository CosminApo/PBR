#include "SDL_manager.h"
#include "Object_manager.h"

/**
The SDL manager will manage all the SDL functions necessary to create and update the output window.

	@param _WINDOW_HEIGHT The height of the SDL Window
	@param _WINDOW_WIDTH The width of the SDL Window
	@param _WINDOW_TITLE The title of the SDL Window
*/
SDL_manager::SDL_manager(const int* _WINDOW_HEIGHT, const int* _WINDOW_WIDTH, std::string* _WINDOW_TITLE )
{
	window_height = *_WINDOW_HEIGHT;
	window_width = *_WINDOW_WIDTH;
	window_title = *_WINDOW_TITLE;
	SDL_Window* window = 0;
	object_up = 0, object_down = 0, object_right = 0, object_left = 0;
	object_speed = 0.1f;
	object_xMove = 0;
	object_yMove = 0;
	camera_up = 0, camera_down = 0, camera_right = 0, camera_left = 0; //movement vars
	camera_speed = 0.1f;
	camera_zMove = 0.2f;
	camera_yMove = 0;
	
	//default light positions
	lightPositions[0] = glm::vec3(-10.f, 0.f, -15.f); 
	lightPositions[1] = glm::vec3(10.f, 0.f, -15.f);  
	lightPositions[2] = glm::vec3(-10.f, 0.f, -15.f);
	lightPositions[3] = glm::vec3(10.f, 0.f, -15.f);
}

/**
Initialise the VIDEO and window components of SDL.
*/
int SDL_manager::initAll()
{
	//Initialise the video component
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "VIDEO INIT FAILED: " << SDL_GetError() << std::endl;
		throw std::exception();
	}

	window = SDL_CreateWindow( //Creat an SDL window
		window_title.c_str(), //with this title
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, //at this position on the screen
		window_width, window_height, //with this width and height
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL); //and set it to be a RESIZABLE, OPENGL window

	if (!SDL_GL_CreateContext(window)) //tell OPENGL where to draw
	{
		std::cout << "Could not create OPENGL context:  " << SDL_GetError() << std::endl;
		throw std::exception();
	}

	//init glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW init failed\n";
		throw std::exception();
	}


	return 0;
}

/**
The main draw loop.
	
	@param _programID  ID of the shader program to execute
	@param _vaoID ID of the VAO where the data needed by the shader program is stored
	@param _textureID ID of the texture associated with the model stored in the VAO
	@param _GL_DEPTH_TEST enable the depth test feature of OPENGL 
	@param _GL_CULL_FACE enable the cull face feature of OPENGL 
	@param _modelLoc location of the model matrix uniform
	@param _projectionLoc location of the projection matrix uniform
	@param _numVerts amount of vertices of the model
	@param _offsetMat the matrix determining the initial position of the model
	@param _objectType either 2D (false) or 3D (true)
	@param _irrMap id of the irradiance map used for PBR
	@param _prefilterMap id of the prefilter map used for PBR
	@param _brdfLUTtex id for the BRDFLUT tex used for PBR
*/
int SDL_manager::draw(GLuint* _programID, bool* _objectType, GLuint* _vaoID, Object _obj, glm::mat4* _offsetMat, size_t* _numVerts, bool* _GL_DEPTH_TEST = 0, bool* _GL_CULL_FACE = 0, unsigned int _irrMap = 0, unsigned int _prefilterMap = 0, unsigned int _brdfLUTtex = 0)
{
	//find uniform locations 
	GLint modelLoc = glGetUniformLocation(*_programID, "u_Model");
	GLint projectionLoc = glGetUniformLocation(*_programID, "u_Projection");
	GLint viewLoc = glGetUniformLocation(*_programID, "u_View");
	GLint camLoc = glGetUniformLocation(*_programID, "u_ViewPos");
	GLint lightLoc = glGetUniformLocation(*_programID, "u_LightPositions");


	//set the uniform 
	glBindAttribLocation(*_programID, 0, "in_Position");
	glBindAttribLocation(*_programID, 1, "a_TexCoord");
	glBindAttribLocation(*_programID, 2, "a_Normal");
	glBindAttribLocation(*_programID, 3, "a_Tangent");
	glBindAttribLocation(*_programID, 4, "a_biTangent");

	//handle inputs
	manageEvents();
	SDL_GetWindowSize(window, &window_width, &window_height); //get the new size of window
	glViewport(0, 0, window_width, window_height); //tell open GL where to draw

	// Instruct OpenGL to use our shader program and our VAO
	glUseProgram(*_programID);
	glBindVertexArray(*_vaoID);
	
	//bind the map locations
	glUniform1i(glGetUniformLocation(*_programID, "u_AlbedoMap"), 0);
	glUniform1i(glGetUniformLocation(*_programID, "u_NormalMap"), 1);
	glUniform1i(glGetUniformLocation(*_programID, "u_MetallicMap"), 2);
	glUniform1i(glGetUniformLocation(*_programID, "u_RoughnessMap"), 3);
	glUniform1i(glGetUniformLocation(*_programID, "u_AoMap"), 4);
	glUniform1i(glGetUniformLocation(*_programID, "u_IrrMap"), 7);
	glUniform1i(glGetUniformLocation(*_programID, "u_PrefilterMap"), 8);
	glUniform1i(glGetUniformLocation(*_programID, "u_BrdfLUT"), 9);
	// Instruct OpenGL to use the textures created
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _obj.textureID_albedo);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, _obj.textureID_normal);
	glActiveTexture(GL_TEXTURE0+2);
	glBindTexture(GL_TEXTURE_2D, _obj.textureID_metallic);
	glActiveTexture(GL_TEXTURE0+3);
	glBindTexture(GL_TEXTURE_2D, _obj.textureID_roughness);
	glActiveTexture(GL_TEXTURE0+4);
	glBindTexture(GL_TEXTURE_2D, _obj.textureID_ao);
	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _irrMap);
	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _prefilterMap);
	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_2D, _brdfLUTtex);
	if (_GL_DEPTH_TEST) glEnable(GL_DEPTH_TEST); //depth test hides object behind other objects


	//prepare the model mat
	glm::mat4 model(1.0f); //create an identity matrix
	model *= *_offsetMat;
	model = glm::translate(model, glm::vec3(object_xMove,object_yMove, 0.0f)); //move the model with arrow keys
	static float angle = 0.f;
	angle += 0.3f;

	//prepare the view mat
	camPos = glm::vec3(camera_yMove, 0, camera_zMove);
	view = glm::lookAt(
		camPos, // Camera pos in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	//prepare the projection mat
	if (*_objectType) //if object is 3D
	{
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0)); //rotate on y axis
		projection = glm::perspective(glm::radians(45.0f), float(window_width) / (float)window_height, 0.1f, 100.f);
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
	}
	else //if object is 2D
	{
		view = glm::mat4(1.0f); //remove view matrix from ortho proj
		projection = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -1.0f, 1.0f);
		model = glm::scale(model, glm::vec3(100, 100, 1));
	}


	// Upload the model matrix
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //value ptr extracts raw data

	// Upload the projection matrix
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Upload the view matrix
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Upload the cam pos 
	glUniformMatrix4fv(camLoc, 1, GL_FALSE, glm::value_ptr(camPos));

	// Upload the light pos
	glUniform3fv(lightLoc, 4, glm::value_ptr(lightPositions[0]));




	// Draw vertices (a triangle)
	glDrawElements(GL_TRIANGLES, _obj.indices.size(), GL_UNSIGNED_INT, 0);



	return 0;
}


/**
Reset the values of all pointers created and destroy the environment.
*/
int SDL_manager::cleanUp()
{
	SDL_DestroyWindow(window);
	SDL_Quit;
	return 0;
}


/**
Handle keyboard inputs
*/
int SDL_manager::manageEvents()
{

	SDL_Event event = { 0 };

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			/* Look for a keypress */
		case SDL_KEYDOWN:
			//set the relevant boolean to true based on what key is pressed
			if (event.key.keysym.sym == SDLK_UP)
			{
				object_up = true;
			}
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				object_down = true;
			}
			if (event.key.keysym.sym == SDLK_LEFT)
			{
				object_left = true;
			}
			if (event.key.keysym.sym == SDLK_RIGHT)
			{
				object_right = true;
			}
			if (event.key.keysym.sym == SDLK_w)
			{
				camera_up = true;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera_down = true;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				camera_left = true;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				camera_right = true;
			}
			break;

		case SDL_KEYUP:
			//set the relevant boolean to false based on what key is pressed
			if (event.key.keysym.sym == SDLK_UP)
			{
				object_up = false;
			}
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				object_down = false;
			}
			if (event.key.keysym.sym == SDLK_LEFT)
			{
				object_left = false;
			}
			if (event.key.keysym.sym == SDLK_RIGHT)
			{
				object_right = false;
			}
			if (event.key.keysym.sym == SDLK_w)
			{
				camera_up = false;
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera_down = false;
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				camera_left = false;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				camera_right = false;
			}
			break;

		case SDL_QUIT:
			quit = true;
			break;
		}
	}

	if (object_up == true)
	{
		object_yMove += object_speed;
	}

	if (object_down == true)
	{
		object_yMove -= object_speed;
	}

	if (object_left == true)
	{
		object_xMove -= object_speed;
	}

	if (object_right == true)
	{
		object_xMove += object_speed;
	}

	if (camera_up == true)
	{

		camera_zMove -= camera_speed;
	}
	if (camera_down == true)
	{

		camera_zMove += camera_speed;
	}

	if (camera_left == true)
	{
		camera_yMove += camera_speed;
		
	}

	if (camera_right == true)
	{
		camera_yMove -= camera_speed;

	}
	return 0;
}

glm::vec3 SDL_manager::getCamPos()
{
	return camPos;
}

glm::mat4 SDL_manager::getViewMat()
{
	return view;
}

glm::mat4 SDL_manager::getProjMat()
{
	return projection;
}

