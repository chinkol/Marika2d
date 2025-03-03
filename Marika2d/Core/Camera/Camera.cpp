#include "Camera.h"

void Mrk::CameraOutput::Start()
{
	ReSize(resolution);
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

void Mrk::Camera::Init()
{
	AddComponent(ComponentFactory::CreateNew<CameraOutput>());

	CameraHut::AddCamera(std::dynamic_pointer_cast<Camera>(shared_from_this()));
}

const std::vector<std::weak_ptr<Mrk::Camera>>& Mrk::CameraHut::GetCameras()
{
	return Instance().cameras;
}

void Mrk::CameraHut::AddCamera(std::weak_ptr<Camera> camera)
{
	if (!camera.expired())
	{
		Instance().cameras.push_back(camera);
		Instance().mainCamera = camera.lock();
	}
}
