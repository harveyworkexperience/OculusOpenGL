#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#ifndef SCENE_H
#define SCENE_H

class Scene 
{
public:
	// Variables
	unsigned int	cubeVAO, cubeVBO;
	unsigned int	skyboxVAO, skyboxVBO;
	unsigned int	cubeTexture;
	unsigned int	cubemapTexture;

	// Constructor & Deconstructor
	Scene();
	~Scene();

	// Methods
	int init();
	void render(Shader shader, Shader skyboxShader, glm::mat4 proj, glm::mat4 viewMat);
};

#endif // SCENE_H