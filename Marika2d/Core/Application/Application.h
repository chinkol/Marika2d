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

struct GLFWwindow;

namespace Mrk
{
    class Window
    {
    public:
        Window(int width, int height, std::string_view title);
        ~Window();
        void Run(const std::function<void()>& externCallBack);
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

    struct AppContext
    {
        // mainwnd
        std::string mainwndTitle = "Marika Window";
        bool mainViewportFillMainWnd = true;
        glm::i32vec2 mainwndBox = { 800, 600 };   // width, height
        glm::vec4 mainViewport = { 0, 0, 800, 600 };   // left, top, width, height

        //project dir
        std::string projDir = "D:/SourceCode/Marika2d/Marika2d/TestProject";

        // callback
        std::vector<std::function<void()>> loopCallBacks;
    };

    class Application : public Singleton<Application>
    {
        MRK_SINGLETON(Application)
    public:
        static void Run();
        static const AppContext& GetAppContext();
        static void SetAppContext(const AppContext& context);

    private:
        Application();
        AppContext  context;
        Window* window;
    };
}


