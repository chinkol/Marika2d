#include "Common/Singleton/Singleton.h"

#include "Core/Config/ConfigSys.h"
#include "Core/Math/Math.h"

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

    class AppConfig : public ConfigGroup
    {
        MRK_CONFIG(AppConfig)
    public:
        Vector2i size = { 800, 600 };
        std::string title = "Marika Engine";
        std::string projDir = "../TestProject";
        std::string fontPath = "c:/Windows/Fonts/msyh.ttc";
    };

    struct AppContext
    {
        // viewport
        bool viewportFill = true;
        glm::vec4 viewport = { 0, 0, 800, 600 };   // left, top, width, height

        // callbacks
        std::function<void()> appInitedCallBack;
        std::function<void()> updateCallBack;
        std::function<void()> drawCallBack;
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

    private:
        AppContext context;
        Window* window;
    };
}