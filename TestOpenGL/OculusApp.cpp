#include "OculusApp.h"

#define FAIL(X) throw std::runtime_error(X)

#ifndef OVR_DEBUG_LOG
#define OVR_DEBUG_LOG(x)
#endif

// OculusTextureBuffer Method Implementations
// ------------------------------------------
OculusTextureBuffer::OculusTextureBuffer(ovrSession session, ovrSizei size, int sampleCount) :
	Session(session),
	ColorTextureChain(nullptr),
	DepthTextureChain(nullptr),
	fboId(0),
	texSize(0, 0) 
{
	successfullyCreated = 0;

	assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

	texSize = size;

	// This texture isn't necessarily going to be a rendertarget, but it usually is.
	assert(session); // Checking for presence of HMD.

	ovrTextureSwapChainDesc desc = {};
	desc.Type = ovrTexture_2D;
	desc.ArraySize = 1;
	desc.Width = size.w;
	desc.Height = size.h;
	desc.MipLevels = 1;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleCount = sampleCount;
	desc.StaticImage = ovrFalse;

	{
		ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &ColorTextureChain);

		int length = 0;
		ovr_GetTextureSwapChainLength(session, ColorTextureChain, &length);

		if (!OVR_SUCCESS(result))
			successfullyCreated = 1;
		else
		{
			for (int i = 0; i < length; ++i)
			{
				GLuint chainTexId;
				ovr_GetTextureSwapChainBufferGL(Session, ColorTextureChain, i, &chainTexId);
				glBindTexture(GL_TEXTURE_2D, chainTexId);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
		}
	}

	desc.Format = OVR_FORMAT_D32_FLOAT;

	{
		ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &DepthTextureChain);

		int length = 0;
		ovr_GetTextureSwapChainLength(session, DepthTextureChain, &length);

		if (!OVR_SUCCESS(result))
			successfullyCreated = 2;
		else
		{
			for (int i = 0; i < length; ++i)
			{
				GLuint chainTexId;
				ovr_GetTextureSwapChainBufferGL(Session, DepthTextureChain, i, &chainTexId);
				glBindTexture(GL_TEXTURE_2D, chainTexId);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
		}
	}

	glGenFramebuffers(1, &fboId);
}

OculusTextureBuffer::~OculusTextureBuffer()
{
	if (ColorTextureChain)
	{
		ovr_DestroyTextureSwapChain(Session, ColorTextureChain);
		ColorTextureChain = nullptr;
	}
	if (DepthTextureChain)
	{
		ovr_DestroyTextureSwapChain(Session, DepthTextureChain);
		DepthTextureChain = nullptr;
	}
	if (fboId)
	{
		glDeleteFramebuffers(1, &fboId);
		fboId = 0;
	}
}

OVR::Sizei OculusTextureBuffer::GetSize() const
{
	return texSize;
}

void OculusTextureBuffer::SetAndClearRenderSurface()
{
	GLuint curColorTexId;
	GLuint curDepthTexId;
	{
		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(Session, ColorTextureChain, &curIndex);
		ovr_GetTextureSwapChainBufferGL(Session, ColorTextureChain, curIndex, &curColorTexId);
	}
	{
		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(Session, DepthTextureChain, &curIndex);
		ovr_GetTextureSwapChainBufferGL(Session, DepthTextureChain, curIndex, &curDepthTexId);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curColorTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, curDepthTexId, 0);

	glViewport(0, 0, texSize.w, texSize.h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void OculusTextureBuffer::UnsetRenderSurface()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
}

void OculusTextureBuffer::Commit()
{
	ovr_CommitTextureSwapChain(Session, ColorTextureChain);
	ovr_CommitTextureSwapChain(Session, DepthTextureChain);
}

// OculusApp Method Implementations
// --------------------------------
OculusApp::OculusApp() {};

OculusApp::~OculusApp() 
{
	if (mirrorFBO) glDeleteFramebuffers(1, &mirrorFBO);
	if (mirrorTexture) ovr_DestroyMirrorTexture(session, mirrorTexture);
	for (int eye = 0; eye < 2; ++eye)
		delete eyeRenderTexture[eye];

	ovr_Destroy(session);
	ovr_Shutdown();
}

int OculusApp::Init()
{
	eyeRenderTexture[0] = nullptr;
	eyeRenderTexture[1] = nullptr;
	mirrorTexture = nullptr;
	mirrorFBO = 0;
	long long frameIndex = 0;

	result = ovr_Initialize(nullptr);
	if (OVR_FAILURE(result)) {
		std::cout << "Failed to initialise result." << std::endl;
		getchar();
		return 1;
	}

	result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result)) {
		std::cout << "Failed to create session." << std::endl;
		getchar();
		ovr_Shutdown();
		return 1;
	}

	hmdDesc = ovr_GetHmdDesc(session);
	windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };

	// Make eye Render buffers
	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
		eyeRenderTexture[eye] = new OculusTextureBuffer(session, idealTextureSize, 1);

		if (!eyeRenderTexture[eye]->ColorTextureChain || !eyeRenderTexture[eye]->DepthTextureChain || eyeRenderTexture[eye]->successfullyCreated != 0)
		{
			std::cout << eyeRenderTexture[eye]->successfullyCreated << ": Failed to create texture for eye " << eye << "." << std::endl;
			return 2;
		}
	}

	memset(&desc, 0, sizeof(desc));
	desc.Width = windowSize.w;
	desc.Height = windowSize.h;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureWithOptionsGL(session, &desc, &mirrorTexture);
	if (!OVR_SUCCESS(result))
	{
		std::cout << "Failed to create texture." << std::endl;
		return 2;// goto Done;
	}

	// Configure the mirror read buffer
	GLuint texId;
	ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &texId);

	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

	return 0;
}

int OculusApp::Render(Scene* TheScene, Shader shader, Shader skyboxShader, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) 
{
	ovrSessionStatus sessionStatus;
	ovr_GetSessionStatus(session, &sessionStatus);
	glm::mat4 glmview;
	glm::mat4 glmproj;
	bool viewInit = false;
	static float Yaw(3.141592f);
	static OVR::Vector3f Pos2(0.0f, 0.0f, -3.0f);

	if (sessionStatus.ShouldQuit)
		return 1;

	if (sessionStatus.ShouldRecenter)
		ovr_RecenterTrackingOrigin(session);

	if (sessionStatus.IsVisible) {

		// Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyePose) may change at runtime.
		ovrEyeRenderDesc eyeRenderDesc[2];
		eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
		eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

		// Get eye poses, feeding in correct IPD offset
		ovrPosef EyeRenderPose[2];
		ovrPosef HmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
									 eyeRenderDesc[1].HmdToEyePose };

		double sensorSampleTime;    // sensorSampleTime is fed into the layer later
		ovr_GetEyePoses(session, frameIndex, ovrTrue, HmdToEyePose, EyeRenderPose, &sensorSampleTime);

		ovrTimewarpProjectionDesc posTimewarpProjectionDesc = {};

		// Render Scene to Eye Buffers
		for (int eye = 0; eye < 2; ++eye)
		{
			// Switch to eye Render target
			eyeRenderTexture[eye]->SetAndClearRenderSurface();

			// Get view matrix
			OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(Yaw);
			OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(EyeRenderPose[eye].Orientation);
			OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
			OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
			OVR::Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);
			OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
			glmview = glm::transpose(glm::make_mat4(&view.M[0][0]));
			viewInit = true;

			// Get projection matrix
			OVR::Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None);
			posTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(proj, ovrProjection_None);
			glmproj = glm::transpose(glm::make_mat4(&proj.M[0][0]));

			// Render scene
			TheScene->Render(shader, skyboxShader, glmproj, glmview);

			// Avoids an error when calling SetAndClearRenderSurface during next iteration.
			// Without this, during the next while loop iteration SetAndClearRenderSurface
			// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
			// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
			eyeRenderTexture[eye]->UnsetRenderSurface();

			// Commit changes to the textures so they get picked up frame
			eyeRenderTexture[eye]->Commit();
		}

		// Do distortion rendering, Present and flush/sync
		ovrLayerEyeFovDepth ld = {};
		ld.Header.Type = ovrLayerType_EyeFovDepth;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
		ld.ProjectionDesc = posTimewarpProjectionDesc;

		for (int eye = 0; eye < 2; ++eye)
		{
			ld.ColorTexture[eye] = eyeRenderTexture[eye]->ColorTextureChain;
			ld.DepthTexture[eye] = eyeRenderTexture[eye]->DepthTextureChain;
			ld.Viewport[eye] = OVR::Recti(eyeRenderTexture[eye]->GetSize());
			ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
			ld.SensorSampleTime = sensorSampleTime;
		}

		ovrLayerHeader* layers = &ld.Header;
		result = ovr_SubmitFrame(session, frameIndex, nullptr, &layers, 1);
		// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
		if (!OVR_SUCCESS(result))
			return 1;

		frameIndex++;
	}

	// OpenGL: Applying mirror texture to glfw window
	// ----------------------------------------------
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GLint w = windowSize.w;
	GLint h = windowSize.h;
	glBlitFramebuffer(
		0, h, w, 0,						// source
		0, 0, SCR_WIDTH, SCR_HEIGHT,	// destination
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return 0;
}