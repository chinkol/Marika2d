#pragma once

#include "Common/Singleton/Singleton.h"

#include "Third/glm/glm.hpp"
#include "Third/glad/include/glad.h"
#include "Third/glfw/include/glfw3.h"
#include "Third/imgui/imgui.h"
#include "Third/imgui/imgui_impl_glfw.h"
#include "Third/imgui/imgui_impl_opengl3.h"

#include <string>
#include <functional>
#include <iostream>

class GLFWwindow;

namespace Mrk
{
	class Window
	{
	public:
        Window(int width, int height, std::string_view title);
        ~Window();
        void Run(const std::function<void()>& customCallback);
        bool ShouldClose() const;
        GLFWwindow* GetWindow() const;

    private:
        void InitGLFW();
        void InitGLAD();
        void InitImGui();
        void Cleanup();

    private:
        GLFWwindow* window;
        int width;
        int height;
        std::string title;
	};

	class Application : public Singleton<Application>
	{

	};
}


