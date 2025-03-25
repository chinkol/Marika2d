#include "InputSys.h"

#include "Third/imgui/imgui.h"

#include <iostream>

void Mrk::InputSys::Update()
{
	MRK_INSTANCE_REF;

	auto mousePos = ImGui::GetMousePos();
	auto mouseX = (int)mousePos.x;
	auto mouseY = (int)mousePos.y;

	for (auto& [_, io] : instance.ios)
	{
		io.isMouseIn = (io.left <= mouseX && mouseX <= io.right && io.top <= mouseY && mouseY <= io.bottom);
	}
}

Mrk::InputIO& Mrk::InputSys::GetInputIO(std::string_view name)
{
	MRK_INSTANCE_REF;

	auto ret = instance.ios.find(name.data());
	if (ret != instance.ios.end())
	{
		return ret->second;
	}
	else
	{
		return instance.ios.emplace(name.data(), InputIO()).first->second;
	}
}

bool Mrk::InputSys::IsKeyDown(InputIO& io, Key key)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsKeyDown((ImGuiKey)key))
	{
		return true;
	}

	return false;
}

bool Mrk::InputSys::IsKeyPress(InputIO& io, Key key)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsKeyPressed((ImGuiKey)key))
	{
		return true;
	}

	return false;
}

bool Mrk::InputSys::IsKeyRelease(InputIO& io, Key key)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsKeyReleased((ImGuiKey)key))
	{
		return true;
	}

	return false;
}

bool Mrk::InputSys::IsMouseDoubleClick(InputIO& io, MouseBtn btn)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsMouseDoubleClicked((ImGuiMouseButton_)btn) && io.isMouseIn)
	{
		for (auto& [_, io_] : instance.ios)
		{
			if (io_.isMouseIn && io_.layer > io.layer)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool Mrk::InputSys::IsMouseRelease(InputIO& io, MouseBtn btn)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsMouseReleased((ImGuiMouseButton_)btn) && io.isMouseIn)
	{
		for (auto& [_, io_] : instance.ios)
		{
			if (io_.isMouseIn && io_.layer > io.layer)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

float Mrk::InputSys::GetMouseWheel(InputIO& io)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return 0;
	}

	auto& imIO = ImGui::GetIO();

	if (imIO.MouseWheel != 0 && io.isMouseIn)
	{
		for (auto& [_, io_] : instance.ios)
		{
			if (io_.isMouseIn && io_.layer > io.layer)
			{
				return 0;
			}
		}

		return imIO.MouseWheel;
	}

	return 0;
}

Mrk::Vector2 Mrk::InputSys::GetMousePos(InputIO& io)
{
	auto imPos = ImGui::GetMousePos();

	return Vector2(imPos.x - (float)io.left, imPos.y - (float)io.top);
}

Mrk::Vector2 Mrk::InputSys::GetMouseDelta(InputIO& io)
{
	if (!io.enable)
	{
		return { 0.0f, 0.0f };
	}

	auto& imIO = ImGui::GetIO();

	return { imIO.MouseDelta.x, imIO.MouseDelta.y };
}

bool Mrk::InputSys::IsMouseDown(InputIO& io, MouseBtn btn)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsMouseDown((ImGuiMouseButton_)btn) && io.isMouseIn)
	{
		for (auto& [_, io_] : instance.ios)
		{
			if (io_.isMouseIn && io_.layer > io.layer)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool Mrk::InputSys::IsMouseClick(InputIO& io, MouseBtn btn)
{
	MRK_INSTANCE_REF;

	if (!io.enable)
	{
		return false;
	}

	if (ImGui::IsMouseClicked((ImGuiMouseButton_)btn) && io.isMouseIn)
	{
		for (auto& [_, io_] : instance.ios)
		{
			if (io_.isMouseIn && io_.layer > io.layer)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void Mrk::InputIO::SetRect(int left, int top, int right, int bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

void Mrk::InputIO::SetLayer(int layer)
{
	this->layer = layer;
}

void Mrk::InputIO::SetEnable(bool enable)
{
	this->enable = enable;
}

bool Mrk::InputIO::IsKeyDown(Key key)
{
	return InputSys::IsKeyDown(*this, key);
}

bool Mrk::InputIO::IsKeyPress(Key key)
{
	return InputSys::IsKeyPress(*this, key);
}

bool Mrk::InputIO::IsKeyRelease(Key key)
{
	return InputSys::IsKeyRelease(*this, key);
}

bool Mrk::InputIO::IsMouseDown(MouseBtn btn)
{
	return InputSys::IsMouseDown(*this, btn);
}

bool Mrk::InputIO::IsMouseClick(MouseBtn btn)
{
	return InputSys::IsMouseClick(*this, btn);
}

bool Mrk::InputIO::IsMouseDoubleClick(MouseBtn btn)
{
	return InputSys::IsMouseDoubleClick(*this, btn);
}

bool Mrk::InputIO::IsMouseRelease(MouseBtn btn)
{
	return InputSys::IsMouseRelease(*this, btn);
}

float Mrk::InputIO::GetMouseWheel()
{
	return InputSys::GetMouseWheel(*this);
}

Mrk::Vector2 Mrk::InputIO::GetMousePos()
{
	return InputSys::GetMousePos(*this);
}

Mrk::Vector2 Mrk::InputIO::GetMouseDelta()
{
	return InputSys::GetMouseDelta(*this);
}
