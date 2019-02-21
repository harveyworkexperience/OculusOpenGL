#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

#include "Scene.h"

#ifndef OCULUS_APP_H
#define OCULUS_APP_H

struct OculusTextureBuffer
{
	// Variables
	ovrSession				Session;
	ovrTextureSwapChain		ColorTextureChain;
	ovrTextureSwapChain		DepthTextureChain;
	GLuint					fboId;
	OVR::Sizei				texSize;
	int						successfullyCreated;

	// Constructor & Deconstructor
	OculusTextureBuffer(ovrSession session, ovrSizei size, int sampleCount);
	~OculusTextureBuffer();

	// Methods
	OVR::Sizei				GetSize() const;
	void					SetAndClearRenderSurface();
	void					UnsetRenderSurface();
	void					Commit();
};

class OculusApp 
{
public:
	// Variables
	OculusTextureBuffer *	eyeRenderTexture[2];
	ovrMirrorTexture		mirrorTexture;
	GLuint					mirrorFBO;
	long long				frameIndex;
	ovrSession				session;
	ovrGraphicsLuid			luid;
	ovrHmdDesc				hmdDesc;
	ovrSizei				windowSize;
	ovrMirrorTextureDesc	desc;
	ovrResult				result;

	// Constructor & Deconstructor
	OculusApp();
	~OculusApp();

	// Methods
	int						Init();
	int						Render(Scene* TheScene, Shader shader, Shader skyboxShader, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);
};

#endif //OCULUS_APP_H