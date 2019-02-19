// Code is a combined version of the code from:
//		* The Oculus SDK Tutorial on the official website
//		* The Oculus SDK OpenGL OculusRoomTiny code
//		* Some OpenGL code from: https://learnopengl.com/

#include "VRApp.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(ovrGraphicsLuid));
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int VRApp::loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// utility function for loading a 2D texture from memory
// -----------------------------------------------------
unsigned int VRApp::loadTextureFromMemory(unsigned char* const buffer, int bufferlen) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load_from_memory(buffer, bufferlen, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load from memory!" << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int VRApp::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

VRApp::VRApp()
{
	TheOculusApp = NULL;
	TheScene = NULL;
	SCR_WIDTH = 1280;
	SCR_HEIGHT = 720;
	deltaTime = 0.0f;
	lastFrame = 0.0f;
	resourcePath = "../../../../Downloads/OpenGL/app_resources/";
};

VRApp::~VRApp() {};

int VRApp::run()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(1280, 720, "App", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to hide our mouse
	// ---------------------------
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader shader(
		(const char*)(resourcePath + "shaders/cubemap.vs").c_str(),
		(const char*)(resourcePath + "shaders/cubemap.fs").c_str()
	);
	//Shader skyboxShader(
	//	(const char*)(resourcePath + "shaders/skybox.vs").c_str(),
	//	(const char*)(resourcePath + "shaders/skybox.fs").c_str()
	//);
	Shader skysphereShader(
		(const char*)(resourcePath + "shaders/skysphere.vs").c_str(),
		(const char*)(resourcePath + "shaders/skysphere.fs").c_str()
	);

	// preparing a scene
	// -----------------
	TheScene = new Scene();
	TheScene->init();

	// load textures
	// -------------
	TheScene->cubeTexture = loadTexture((const char*)(resourcePath + "textures/container.jpg").c_str());

	//std::vector<std::string> faces
	//{
	//	(resourcePath + "textures/skybox/right.jpg"),
	//	(resourcePath + "textures/skybox/left.jpg"),
	//	(resourcePath + "textures/skybox/top.jpg"),
	//	(resourcePath + "textures/skybox/bottom.jpg"),
	//	(resourcePath + "textures/skybox/front.jpg"),
	//	(resourcePath + "textures/skybox/back.jpg")
	//};
	//TheScene->cubemapTexture = loadCubemap(faces);

	unsigned int t1 = loadTexture((const char*)(resourcePath + "textures/img2.jpg").c_str());
	unsigned int t2 = loadTexture((const char*)(resourcePath + "textures/earth.jpg").c_str());
	TheScene->skysphereTexture = t1;
	//TheScene->skysphereTexture = loadTextureFromMemory((unsigned char* const)pvrtcBytes, 96660);

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);

	//skyboxShader.use();
	//skyboxShader.setInt("skybox", 0);

	skysphereShader.use();
	skysphereShader.setInt("skysphere", 0);

	// Oculus: initialising and configuring
	// ------------------------------------
	TheOculusApp = new OculusApp();
	if (TheOculusApp->init() > 0)
		goto Done;

	// Turn off vsync to let the compositor do its magic
	glfwSwapInterval(0);

	// glfw: render loop
	// -----------------
	while (!glfwWindowShouldClose(window))
	{
		// glfw: per-frame time logic
		// --------------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// OpenGL: rendering scene
		// -----------------------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Oculus: handling session status and rendering to HMD
		// ----------------------------------------------------
		glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
		if (TheOculusApp->render(TheScene, shader, skysphereShader, SCR_WIDTH, SCR_HEIGHT) > 0)
			break;

		// glfw: input
		// -----------
		processInput(window);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// de-allocate all resources once they've outlived their purpose:
	// --------------------------------------------------------------
Done:
	delete(TheScene);
	delete(TheOculusApp);

	glfwTerminate();

	std::cout << "Press ENTER to exit." << std::endl;
	getchar();
	return 0;
}