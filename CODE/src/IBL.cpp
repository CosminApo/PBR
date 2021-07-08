#include "IBL.h"


IBL::IBL() : currentFragmentShader("temp"), currentVertexShader("temp"), myShaderProgram(), myTextureManager((std::string("temp")))
{

}

/**
Sets up the IBL maps and shaders.
	@param _path the path of the HDR texture to load
*/
void IBL::addIBL(std::string _path)
{
	//set up the skybox shader
	Fragment_shader fragmentShader(&std::string("IBL_skyboxfrag.glsl"));
	Vertex_shader vertexShader(&std::string("IBL_skyboxvec.glsl"));
	GLuint vertexShaderID = 0;
	GLuint fragmentShaderID = 0;
	fragmentShader.compileShader(&fragmentShaderID);
	vertexShader.compileShader(&vertexShaderID);
	//compile and activate shader program
	myShaderProgram.compileShaderProgram(&skybox_programID, &vertexShaderID, &fragmentShaderID);


	//set up the hdrtocube shader
	Fragment_shader fragmentShader_hdrtc(&std::string("IBL_fragmentshader.glsl"));
	Vertex_shader vertexShader_hdrtc(&std::string("IBL_vertexshader.glsl"));
	GLuint vertexShaderID2 = 0;
	GLuint fragmentShaderID2 = 0;
	fragmentShader_hdrtc.compileShader(&fragmentShaderID2);
	vertexShader_hdrtc.compileShader(&vertexShaderID2);
	//compile and activate shader program
	myShaderProgram.compileShaderProgram(&hdrtocube_programID, &vertexShaderID2, &fragmentShaderID2);

	//set up the irradiance shader
	Fragment_shader fragmentShader_irr(&std::string("IBL_irradiancefrag.glsl"));
	Vertex_shader vertexShader_irr(&std::string("IBL_irradiancevec.glsl"));
	GLuint vertexShaderID3 = 0;
	GLuint fragmentShaderID3 = 0;
	fragmentShader_irr.compileShader(&fragmentShaderID3);
	vertexShader_irr.compileShader(&vertexShaderID3);
	//compile and activate shader program
	myShaderProgram.compileShaderProgram(&irradiance_programID, &vertexShaderID3, &fragmentShaderID3);


	//set up the importancemapping shader
	Fragment_shader fragmentShader_imps(&std::string("IBL_irradiancefrag.glsl"));
	Vertex_shader vertexShader_imps(&std::string("IBL_irradiancevec.glsl"));
	GLuint vertexShaderID4 = 0;
	GLuint fragmentShaderID4 = 0;
	fragmentShader_imps.compileShader(&fragmentShaderID4);
	vertexShader_imps.compileShader(&vertexShaderID4);
	//compile and activate shader program
	myShaderProgram.compileShaderProgram(&impsample_programID, &vertexShaderID4, &fragmentShaderID4);

	//set up the brdf shader
	Fragment_shader fragmentShader_brdf(&std::string("IBL_brdfintegrationfrag.glsl"));
	Vertex_shader vertexShader_brdf(&std::string("IBL_brdfintegrationvec.glsl"));
	GLuint vertexShaderID5 = 0;
	GLuint fragmentShaderID5 = 0;
	fragmentShader_brdf.compileShader(&fragmentShaderID5);
	vertexShader_brdf.compileShader(&vertexShaderID5);
	//compile and activate shader program
	myShaderProgram.compileShaderProgram(&brdfimpl_programID, &vertexShaderID5, &fragmentShaderID5);

	//enalbe skybox program
	glUseProgram(skybox_programID);
	glUniform1i(glGetUniformLocation(skybox_programID, "environmentMap"), 0);

	//set up frame buffer and frame buffer object
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 800, 800);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	//load in texture
	GLuint hdrtex;
	myTextureManager = *(new Texture_manager(_path));
	myTextureManager.generateHDR(&hdrtex, &skybox_programID, _path);

	//set up cubemap and link it to frame buffer
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 800, 800, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//set up object to world space matrices
	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	//need 6 view matrices cause its a cube
	captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));


	//convert HDR map to cube map
	//use hdrtocube program
	glUseProgram(hdrtocube_programID);
	glUniform1i(glGetUniformLocation(hdrtocube_programID, "equirectangularMap"), 0);
	glUniformMatrix4fv(glGetUniformLocation(hdrtocube_programID, "projection"), 1, GL_FALSE, &captureProjection[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrtex);
	glViewport(0, 0, 800, 800); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(hdrtocube_programID, "view"), 1, GL_FALSE, &captureViews[i][0][0]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube(); // renders a 1x1 cube


	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//create irradiance texture
	//use irradiance map generator program
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		//this texture can be smaller so 32 32 is fine
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	glUseProgram(irradiance_programID);
	glUniform1i(glGetUniformLocation(irradiance_programID, "environmentMap"), 0);
	glUniformMatrix4fv(glGetUniformLocation(irradiance_programID, "projection"), 1, GL_FALSE, &captureProjection[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(irradiance_programID, "view"), 1, GL_FALSE, &captureViews[i][0][0]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//Prefilter the HDR map
	//use prefilter map generator program
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr); //128 is enough for most objs
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //linear cause we sample it later
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glUseProgram(impsample_programID);
	glUniform1i(glGetUniformLocation(impsample_programID, "environmentMap"), 0);
	glUniformMatrix4fv(glGetUniformLocation(impsample_programID, "projection"), 1, GL_FALSE, &captureProjection[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	//set up map using quasi montecarlos
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(glGetUniformLocation(impsample_programID, "roughness"), roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glUniformMatrix4fv(glGetUniformLocation(impsample_programID, "view"), 1, GL_FALSE, &captureViews[i][0][0]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Integrate BRDF
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	glUseProgram(brdfimpl_programID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//glDeleteProgram(programID);
}


/**
Render the IBL skybox.
	@param _sdlmanager the sdl manager needed to have a window to draw on
	@param _view the view matrix to use when drawing. has to be same as the obj manager one
	@param _projection the projection matrix to use when drawing. has to be same as the obj manager one

*/
void IBL::renderIBL(SDL_manager *_sdlmanager, glm::mat4 _view, glm::mat4 _projection)
{

	glDepthFunc(GL_LEQUAL);

	//activate shader
	glUseProgram(skybox_programID);


	GLint viewLoc = glGetUniformLocation(skybox_programID, "view");
	// Upload the view matrix
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_view));



	GLint projectionLoc = glGetUniformLocation(skybox_programID, "projection");
	// Upload the projection matrix
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(_projection));

	//bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	//render the cube
	renderCube();

	glDepthFunc(GL_LESS);

}

/**
Renders a 1x1 cube
*/
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void IBL::renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		unsigned int skyboxVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(cubeVAO);

		//setup VBO
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		//upload the data
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		//fill the vao
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		//reset
		glBindVertexArray(0);
		glDeleteBuffers(1, &skyboxVBO);

		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

/**
Renders a quad
*/
unsigned int quadVAO = 0;
unsigned int quadVBO;
void IBL::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		//set up vao
		glGenVertexArrays(1, &quadVAO);
		//set up vbo
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		//upload data to vbo
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		//upload data to vao
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


unsigned int IBL::getirradianceMap()
{
	return irradianceMap;
}

unsigned int IBL::getprefilteredMap()
{
	return prefilterMap;
}

unsigned int IBL::getLutTex()
{
	return brdfLUTTexture;
}

