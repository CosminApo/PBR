#include <iostream>
#include <exception>
#include <vector>
#include "SDL_manager.h"
#include <string>
#include "Object_manager.h"
#include "IBL.h"

const int FPS = 144;
const int frameDelay = 1000 / FPS;
GLuint m_screenQuadVAO;
int main(int argc, char* argv[])
{
	//prep sdl
	int window_width = 800;
	int window_height = 800;
	std::string title = "Render";
	SDL_manager mySDLManager(&window_width, &window_height, &title);
	mySDLManager.initAll();
	int frameTime = 0;
	Uint32 frameStart = 0;

	//create shaders
	Fragment_shader fragmentShader(&std::string("Fragment_shader_PBR.glsl"));
	Vertex_shader vertexShader(&std::string("Vertex_shader_PBR.glsl"));

	//Prepare IBL
	IBL iblm;

	//Add the IBL map
	iblm.addIBL("Circus_Backstage/Circus_Backstage_3k.hdr");


	//Prepare object manager
	Object_manager myObjectManager;
	myObjectManager.addObject( //create a new object
		true, //type 3D
		"oni.obj", //with this model
		fragmentShader, //this fragment shader
		vertexShader, //this vertex shader
		glm::vec3(0.0f, .0f, -20.f)); //at this coordinates



	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); for debug only
	//Prepare the PBR shader
	GLuint shaderf;
	fragmentShader.compileShader(&shaderf);
	GLuint shaderv;
	vertexShader.compileShader(&shaderv);
	GLuint programv;
	myObjectManager.myShaderProgram.compileShaderProgram(&programv, &shaderv, &shaderf);
	myObjectManager.setID(programv);

	//Set IBM related parameters to the objectmanager
	myObjectManager.setIrrMap(iblm.getirradianceMap());
	myObjectManager.setprefilterMap(iblm.getprefilteredMap());
	myObjectManager.setBrdfLutTex(iblm.getLutTex());

	//toggle this to fix some IBL issues
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//main loop
	while (true)
	{
		//clear the screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    frameStart = SDL_GetTicks();

		//draw object
		myObjectManager.drawAll(true,true,&mySDLManager);

		//draw ibl skybox
		iblm.renderIBL(&mySDLManager, mySDLManager.getViewMat(), mySDLManager.getProjMat());


		//frame lock at 60fps
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}

		//refresh
		SDL_GL_SwapWindow(mySDLManager.getWindow());
	}


	
	return 0;
}


