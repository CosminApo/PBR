#ifndef _OBJECT_MANAGER_H
#define _OBJECT_MANAGER_H

#include <iostream> /*error logging*/
#include <string>
#include "Vertex_shader.h"
#include "Fragment_shader.h"
#include "Program_manager.h"

class Object_manager
{
	struct Object
	{
		bool objectType; //true = 3d false = 2d
		std::string modelSrc; //path to file

	};
};

#endif // !_OBJECT_MANAGER_H