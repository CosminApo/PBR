#ifndef SDL_MANAGER_H
#define SDL_MANAGER_H

#include <SDL2/SDL.h> 
#include <iostream> /*Console error logging*/
#include <GL/glew.h> /*Open GL drawing operations*/
#include <glm/glm.hpp> /*Math functions*/
#include <glm/ext.hpp>  /*Even more Math functions (matrix related)*/
#include <exception>
#include <vector>
#include <string>
#include "Object.h"

class SDL_manager
{
private:
	bool quit;
	int window_height;
	int window_width;
	std::string window_title;
	SDL_Window* window;
	bool object_up, object_down, object_right, object_left; //movement vars
	float object_speed;
	float object_xMove;
	float object_yMove;

	bool camera_up, camera_down, camera_right, camera_left; //movement vars
	float camera_speed;
	float camera_zMove;
	float camera_yMove;
	glm::vec3 camPos;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightPositions[4];

public:
	SDL_manager(const int* _WINDOW_HEIGHT, const int* _WINDOW_WIDTH, std::string* _WINDOW_TITLE);
	int initAll();
	int draw(GLuint* _programID, bool* _objectType, GLuint* _vaoID, Object _obj, glm::mat4* _offsetMat,size_t* _numVerts, bool* _GL_DEPTH_TEST, bool* _GL_CULL_FACE, unsigned int _irrMap, unsigned int _prefilterMap, unsigned int _brdfLUTtex);
	int cleanUp();
	int manageEvents();
	SDL_Window* getWindow() { return window; }
	glm::vec3 getCamPos();
	glm::mat4 getViewMat();
	glm::mat4 getProjMat();
};


#endif // !SDL_MANAGER_H