#include "Application.h"

#include "Core/Component/Component.h"

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
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 19.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void Mrk::Window::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
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
    auto& context = Instance().context;
    auto window = Instance().window;
    window = new Window(context.windowSize.x, context.windowSize.y, context.windowTitle);

    if (context.appInitedCallBack) context.appInitedCallBack();

    window->Run([context]()
        {
            //启动 ImGui 帧
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            Mrk::ComponentHouse::Invoke("PreUpdate");
            Mrk::ComponentHouse::Invoke("Update");
            Mrk::ComponentHouse::Invoke("LateUpdate");
            Mrk::ComponentHouse::Invoke("FixedUpdate");    //TODO : 改定时
            if (context.updateCallBack) context.updateCallBack();
            Mrk::ComponentHouse::Invoke("PreDraw");
            Mrk::ComponentHouse::Invoke("Draw");
            Mrk::ComponentHouse::Invoke("LateDraw");
            if (context.drawCallBack) context.drawCallBack();

            // 渲染 ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        });

    delete window;
}

const Mrk::AppContext& Mrk::Application::GetAppContext()
{
    return Instance().context;
}

void Mrk::Application::SetAppContext(const AppContext& context)
{
    Instance().context = context;
}