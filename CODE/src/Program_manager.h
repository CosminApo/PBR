#ifndef _PROGRAM_MANAGER_H
#define _PROGRAM_MANAGER_H

#include <GL/glew.h> /*Open GL drawing operations*/
#include <vector>
#include <iostream> /*Console error logging*/

class Program_manager
{
private:
public:
	int compileShaderProgram(GLuint* _programID, GLuint* _vertexShaderID, GLuint* _fragmentShaderID);
};
#endif // !_PROGRAM_MANAGER_H
