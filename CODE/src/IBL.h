#include <string> /*file paths and error logging*/
#include "Vertex_shader.h"
#include "Fragment_shader.h"
#include "Program_manager.h"
#include "Texture_manager.h" /*texture loading*/
#include "SDL_manager.h" /*window manager for windows*/
#include "Texture_manager.h" /*texture loading*/

class IBL
{
private:
	Fragment_shader currentFragmentShader;
	Vertex_shader currentVertexShader;
	GLuint vertexShaderID = 0;
	GLuint fragmentShaderID = 0;
	Program_manager myShaderProgram;
	GLuint hdrtocube_programID = 0;
	GLuint skybox_programID = 0;
	GLuint irradiance_programID = 0;
	GLuint impsample_programID = 0;
	GLuint brdfimpl_programID = 0;
	Texture_manager myTextureManager;
	glm::mat4 captureProjection;
	glm::mat4 captureViews[6];
	GLuint hdrtex;
	unsigned int captureFBO, captureRBO, envCubemap, irradianceMap, prefilterMap, brdfLUTTexture;
public:
	IBL();
	void addIBL(std::string _path);
	void renderIBL(SDL_manager *_sdlmanager, glm::mat4 _view, glm::mat4 _projection);
	void renderCube();
	void renderQuad();
	unsigned int getirradianceMap();
	unsigned int getprefilteredMap();
	unsigned int getLutTex();

};

