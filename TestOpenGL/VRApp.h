#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

#include "shader.h"
#include "Scene.h"
#include "OculusApp.h"

#include <iostream>
#include <algorithm>
#include <vector> 
#include <string>
#include <thread>

#ifndef VR_APP_H
#define VR_APP_H

class VRApp
{
public:
	// Variables
	OculusApp*				TheOculusApp;
	Scene*					TheScene;
	int						SCR_WIDTH;
	int						SCR_HEIGHT;
	float					deltaTime;
	float					lastFrame;
	std::string				resourcePath;

	// Constructor & Deconstructor
	VRApp();
	~VRApp();

	// Methods
	unsigned int loadTexture(const char *path);
	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int loadTextureFromMemory(unsigned char* buffer, int bufferlen);
	int run();
};

#endif //VR_APP_H