#include "Camera.h"

#include "Core/Mesh/Mesh.h"

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

	if (!holder.expired())
	{
		trans = holder.lock()->GetComponent<Transform>();
	}
}

void Mrk::CameraOutput::PreDraw()
{
	currBackBufferIndex = (currBackBufferIndex + 1) % 2;
}

const Mrk::Vector2i& Mrk::CameraOutput::GetResolution() const
{
	return resolution;
}

void Mrk::CameraOutput::SetResolution(const Vector2i& resolution)
{
	this->resolution = resolution;
	ReSize(resolution);
}

const Mrk::CameraFrustum& Mrk::CameraOutput::GetFrustum() const
{
	return frustum;
}

void Mrk::CameraOutput::SetFrustum_(const CameraFrustum& frumstum)
{
	this->frustum = frumstum;
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


	//TODO : replace me
	{
		if (!renderLayers[1].empty())
		{
			glUseProgram(renderLayers[1][0].sp);
		}
	}

	//camera

	if (trans)
	{
		auto view = trans->GetViewMatrix();
		auto proj = frustum.GetProjMatrix();
		auto viewproj = proj * view;

		glUniformMatrix4fv(MRK_GL_LOCATING_VIEW, 1, GL_FALSE, (GLfloat*)&view);
		glUniformMatrix4fv(MRK_GL_LOCATING_PROJ, 1, GL_FALSE, (GLfloat*)&proj);
		glUniformMatrix4fv(MRK_GL_LOCATING_VIEWPROJ, 1, GL_FALSE, (GLfloat*)&viewproj);
	}

	//light
	//......

	//clear
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
	resolution = { std::max(0, newSize.x), std::max(0, newSize.y) };

	frustum.SetAspect((float)resolution.x / (float)resolution.y);

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
		//glUseProgram(item.sp);

		item.mesh->Bind();

		glUniform2ui(MRK_GL_LOCATING_OBJECTID, item.id.low32, item.id.high32);
		glUniformMatrix4fv(MRK_GL_LOCATING_WORLD, 1, GL_FALSE, (GLfloat*)&item.world);

		auto subMeshes = item.mesh->GetSubMeshes();
		for (size_t i = 0; i < subMeshes.size(); i++)
		{
			auto& subMesh = subMeshes[i];
			glDrawElements(GL_TRIANGLES, subMesh.count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(unsigned int) * subMesh.offset));
		}
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

float Mrk::CameraFrustum::GetNearZ() const
{
	return nearZ;
}

void Mrk::CameraFrustum::SetNearZ(float nearZ)
{
	isDirty = true;
	this->nearZ = nearZ;
}

float Mrk::CameraFrustum::GetFarZ() const
{
	return farZ;
}

void Mrk::CameraFrustum::SetFarZ(float farZ)
{
	isDirty = true;
	this->farZ = farZ;
}

float Mrk::CameraFrustum::GetAngle() const
{
	return angle;
}

void Mrk::CameraFrustum::SetAngle(float angle)
{
	isDirty = true;
	this->angle = angle;
}

float Mrk::CameraFrustum::GetAspect() const
{
	return aspect;
}

void Mrk::CameraFrustum::SetAspect(float aspect)
{
	isDirty = true;
	this->aspect = aspect;
}

const Mrk::Matrix4& Mrk::CameraFrustum::GetProjMatrix()
{
	if (isDirty)
	{
		proj = glm::perspective(glm::radians(angle), aspect, nearZ, farZ);
		isDirty = false;
	}

	return proj;
}

void Mrk::CameraController::Start()
{
	assert(!holder.expired());

	trans = holder.lock()->GetComponent<Transform>();
}

#include "Third/imgui/imgui.h"

void Mrk::CameraController::Update()
{
	auto& io = ImGui::GetIO();

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		trans->RotateWorld(io.MouseDelta.x * 0.1f, { 0.0f, -1.0f, 0.0f });
		trans->RotateLocal(io.MouseDelta.y * 0.1f, { 1.0f, 0.0f, 0.0f });
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
	{
		trans->TranslateLocal({ 0.025f * io.MouseDelta.x, 0.025f * io.MouseDelta.y, 0 });
	}

	if (io.MouseWheel != 0)
	{
		trans->TranslateLocal({ 0.0f, 0.0f, io.MouseWheel });
	}
}

bool Mrk::CameraController::FindCamera()
{
	if (!cameraTrans)
	{
		//TODO:
		auto& children = holder.lock()->GetChildren();
		for (auto& child : children)
		{
			if (child && child->GetClassTypeName() == "Camera")
			{
				cameraTrans = child->GetComponent<Transform>();
				return true;
			}
		}
	}

	return false;
}
