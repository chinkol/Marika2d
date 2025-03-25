#include "Application.h"

#include "Core/Component/Component.h"
#include "Core/Render/Render.h"
#include "Core/Input/InputSys.h"

#include "Third/imgui/imgui.h"
#include "Third/imgui/imgui_impl_glfw.h"
#include "Third/imgui/imgui_impl_opengl3.h"
#include "Third/imgui/freetype/imgui_freetype.h"
#include "Third/imnodes/imnodes.h"

Mrk::Window::Window(int width, int height, std::string_view title) :
    width(width),
    height(height),
    title(title)
{
    InitGLFW();
    InitGLAD();
    InitImGui();
}

Mrk::Window::~Window()
{
    Cleanup();
}

void Mrk::Window::Run(const std::function<void()>& externCallBack)
{
    while (!ShouldClose())
    {
        glClearColor(0.1f, 0.2f, 0.3f, 0.4f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        externCallBack();

        // 交换缓冲区
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool Mrk::Window::ShouldClose() const
{
    return glfwWindowShouldClose(window);
}

GLFWwindow* Mrk::Window::GetWindow() const
{
    return window;
}

void Mrk::Window::InitGLFW()
{
    if (!glfwInit())
    {
        throw; //std::runtime_error("Failed to initialize GLFW.");
    }

    // 配置 OpenGL 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        });

    // 创建窗口
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw; //std::runtime_error("Failed to create GLFW window.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 开启垂直同步
}

void Mrk::Window::InitGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw; // std::runtime_error("Failed to initialize GLAD.");
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
}

void Mrk::Window::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImFontConfig font_config;
    font_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_Bold | ImGuiFreeTypeBuilderFlags_LoadColor;
    io.Fonts->AddFontFromFileTTF(ConfigSys::GetConfigItem<std::string>("AppConfig", "fontPath").c_str(), 16.0f, &font_config, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();

    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.85f, 0.75f, 1.0f);
}

void Mrk::Window::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

Mrk::Application::Application() :
    window(nullptr)
{
}

void Mrk::Application::Run()
{
    auto size = ConfigSys::GetConfigItem<Vector2i>("AppConfig", "size");
    auto title = ConfigSys::GetConfigItem<std::string>("AppConfig", "title");

    Instance().window = new Window(size.x, size.y, title);

    auto& context = Instance().context;

    if (context.appInitedCallBack)
        context.appInitedCallBack();

    Instance().window->Run([context]()
        {
            //启动 ImGui 帧
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            Mrk::InputSys::Update();

            Mrk::ComponentHut::Invoke("PreUpdate");
            Mrk::ComponentHut::Invoke("Update");
            Mrk::ComponentHut::Invoke("LateUpdate");
            Mrk::ComponentHut::Invoke("FixedUpdate");    //TODO : 改定时

            if (context.updateCallBack)
                context.updateCallBack();

            Mrk::ComponentHut::Invoke("PreDraw");
            Mrk::ComponentHut::Invoke("Draw");
            Mrk::RenderSys::Draw();
            Mrk::ComponentHut::Invoke("LateDraw");

            if (context.drawCallBack)
                context.drawCallBack();

            // 渲染 ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        });

    delete Instance().window;
}

const Mrk::AppContext& Mrk::Application::GetAppContext()
{
    return Instance().context;
}

void Mrk::Application::SetAppContext(const AppContext& context)
{
    Instance().context = context;
}