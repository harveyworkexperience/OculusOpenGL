#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Sphere.h"

#ifndef SCENE_H
#define SCENE_H

class Scene 
{
public:
	// Variables
	unsigned int	cubeVAO, cubeVBO;
	unsigned int	skyboxVAO, skyboxVBO;
	unsigned int	skysphereVAO, skysphereVBO;
	unsigned int	cubeTexture, cubemapTexture, skysphereTexture;
	Sphere*			sphere;

	// Constructor & Deconstructor
	Scene();
	~Scene();

	// Methods
	int				Init();
	void			Render(Shader shader, Shader skyShader, glm::mat4 proj, glm::mat4 viewMat);
};

#endif // SCENE_H