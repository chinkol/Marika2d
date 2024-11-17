#include "Application.h"

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

void Mrk::Window::Run(const std::function<void()>& customCallback)
{
    while (!ShouldClose())
    {
        // ����
        glClearColor(0.1f, 0.2f, 0.3f, 0.4f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ���� ImGui ֡
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // �û��������Ⱦ�ص�
        customCallback();

        //imgui demo
        ImGui::ShowDemoWindow();

        // ��Ⱦ ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // ����������
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

    // ���� OpenGL �汾
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        });

    // ��������
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw; //std::runtime_error("Failed to create GLFW window.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // ������ֱͬ��
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
    (void)io;

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
