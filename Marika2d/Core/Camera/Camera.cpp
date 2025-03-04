#include "Camera.h"

Mrk::CameraOutput::~CameraOutput()
{
	for (size_t i = 0; i < 2; i++)
	{
		glDeleteFramebuffers(1, &backBuffers[i]);
		glDeleteTextures(1, &backBufferTextures[i]);
		glDeleteTextures(1, &depthBufferTextures[i]);
		glDeleteTextures(1, &idTextures[i]);
	}
}

void Mrk::CameraOutput::Start()
{
	ReSize(resolution);
}

void Mrk::CameraOutput::PreDraw()
{
	currBackBufferIndex = (currBackBufferIndex + 1) % 2;
}

const Mrk::Vector2i& Mrk::CameraOutput::GetResolution()
{
	return resolution;
}

void Mrk::CameraOutput::SetResolution(const Vector2i& resolution)
{
	this->resolution = resolution;
	//TODO : Resize
}

GLuint Mrk::CameraOutput::GetBackBuffer()
{
	return backBufferTextures[currBackBufferIndex];
}

GLuint Mrk::CameraOutput::GetIdTexture()
{
	return idTextures[currBackBufferIndex];
}

GLuint Mrk::CameraOutput::GetBackBuffertexture()
{
	return backBufferTextures[currBackBufferIndex];
}

GLuint Mrk::CameraOutput::GetDepthBuffertexture()
{
	return depthBufferTextures[currBackBufferIndex];
}

void Mrk::CameraOutput::Shot(const std::array<std::vector<RenderItem>, 4>& renderLayers)
{
	//bind backbuffer
	auto backbuffer = backBuffers[currBackBufferIndex];
	glBindFramebuffer(GL_FRAMEBUFFER, backbuffer);

	//camera
	//glUniform4fv(0, );
	//glUniform4fv(1, );
	//glUniform4fv(2, );

	//light
	//......

	//clear
	//gl->glEnable()
	glViewport(0, 0, resolution.x, resolution.y);
	glClearColor(190.0f * 0.9f / 255.0f, 237.0f * 0.9f / 255.0f, 199.0f * 0.9f / 255.0f, 1.0f);
	GLuint clearColor[] = { 0, 0, 0, 0 };
	glClearBufferuiv(GL_COLOR, 2, clearColor);
	glClearBufferuiv(GL_COLOR, 1, clearColor);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//background
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Shot(renderLayers[(int)RenderLayer::BackGround]);

	//geometry
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Shot(renderLayers[(int)RenderLayer::Geometry]);

	//transparent
	//......

	//foreground
	//......

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glFlush();
}

void Mrk::CameraOutput::ReSize(const Vector2i& newSize)
{
	resolution = { std::max(800, resolution.x), std::max(600, resolution.y) };

	//unbinding
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//create new buffers and textures
	for (size_t i = 0; i < 2; i++)
	{
		//framebuffer
		auto& backbuffer = backBuffers[i];
		glDeleteFramebuffers(1, &backbuffer);
		glGenFramebuffers(1, &backbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, backbuffer);

		//back buffer texture
		auto& backbuferTexture = backBufferTextures[i];
		glDeleteTextures(1, &backbuferTexture);
		glGenTextures(1, &backbuferTexture);
		glBindTexture(GL_TEXTURE_2D, backbuferTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, resolution.x, resolution.y);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backbuferTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//depth buffer texture
		auto& depthbufferTexture = depthBufferTextures[i];
		glDeleteTextures(1, &depthbufferTexture);
		glGenTextures(1, &depthbufferTexture);
		glBindTexture(GL_TEXTURE_2D, depthbufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbufferTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//id texture
		auto& idtexture = idTextures[i];
		glDeleteTextures(1, &idtexture);
		glGenTextures(1, &idtexture);
		glBindTexture(GL_TEXTURE_2D, idtexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32UI, resolution.x, resolution.y);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, idtexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//mtr
		GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		//check
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			throw; //framebuffer error
		}

		// 完成后解绑
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Mrk::CameraOutput::Shot(const std::vector<RenderItem>& renderItems)
{ 
	for (auto& item : renderItems)
	{
		
	}
}

void Mrk::Camera::Init()
{
	AddComponent(ComponentFactory::CreateNew<CameraOutput>());

	CameraHut::AddCamera(std::dynamic_pointer_cast<Camera>(shared_from_this()));
}

std::shared_ptr<Mrk::Camera> Mrk::CameraHut::GetMainCamera()
{
	MRK_INSTANCE_REF;

	if (!instance.mainCamera.expired())
	{
		return instance.mainCamera.lock();
	}
	return std::shared_ptr<Mrk::Camera>();
}

const std::vector<std::weak_ptr<Mrk::Camera>>& Mrk::CameraHut::GetCameras()
{
	MRK_INSTANCE_REF;

	return instance.cameras;
}

void Mrk::CameraHut::AddCamera(std::weak_ptr<Camera> camera)
{
	MRK_INSTANCE_REF;

	if (!camera.expired())
	{
		instance.cameras.push_back(camera);
		instance.mainCamera = camera.lock();
	}
}