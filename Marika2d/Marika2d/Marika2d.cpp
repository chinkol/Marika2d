// Marika2d.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <sstream>

#include "Third/box2d/include/box2d.h"
#include "Third/glad/include/glad.h"
#include "Third/glfw/include/glfw3.h"

#include "Marika2d/Common/Log/LogSystem.h"
#include "Marika2d/Common/Serialize/SerializeSystem.h"

class BaseClass
{
public:
    virtual inline void VirFunc()
    {
        std::cout << "BaseClass::VirFunc" << "\n";
    }
};

class TestClass : public BaseClass, public Mrk::ISerializable
{
    MRK_SERIALIZABLE(TestClass)
public:
    virtual inline void Serialize(Mrk::OutSerializeStream* stream)
    {
        MRK_WRITE_FIELD(field1);
        MRK_WRITE_FIELD(field2);
        MRK_WRITE_FIELD(field3);
        MRK_WRITE_FIELD(field4);
    }
    virtual inline void DeSerialize(Mrk::InSerializeStream* stream)
    {
        MRK_READ_FIELD(field1, field1);
        MRK_READ_FIELD(field2, field2);
        MRK_READ_FIELD(field3, field3);
        MRK_READ_FIELD(field4, field4);
    }

public:
    virtual inline void VirFunc()
    {
        std::cout << "TestClass::VirFunc" << "\n";
    }

public:
    int field1 = 0;
    float field2 = 0;
    double field3 = 0;
    std::string field4 = "NULL";
};

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 窗口大小变化时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

int main() 
{
    Mrk::LogSystem::Init(Mrk::LogTargetType::Console);
    MRK_LOG_ERROR("Test Error");
    Mrk::LogSystem::Log("12345");

    TestClass* test = new TestClass();
    test->field1 = 12345;
    test->field2 = 1.2345f;
    test->field3 = 1.23456789;
    test->field4 = "Hello World";

    Mrk::SerializeStream stream;

    Mrk::OutSerializeStream os(&stream);
    test->Serialize(&os);

    Mrk::InSerializeStream is(&stream);
    TestClass* test2 = new TestClass();
    test2->DeSerialize(&is);

    std::istringstream input("asdca");
    std::string k = "hello";
    input >> k;

    //Mrk::SerializeManager::GetInstance().RegisteClass<TestClass>("TestClass");
    auto test3 = Mrk::SerializeManager::GetInstance().Create("TestClass");

    BaseClass* base = dynamic_cast<BaseClass*>(test3);
    base->VirFunc();

    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "无法初始化 GLFW" << std::endl;
        return -1;
    }

    // 设置 GLFW 使用的 OpenGL 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建 GLFW 窗口
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW + GLAD 窗口", nullptr, nullptr);
    if (!window) {
        std::cerr << "无法创建 GLFW 窗口" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 设置当前上下文
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "无法初始化 GLAD" << std::endl;
        return -1;
    }

    // 设置视口
    glViewport(0, 0, WIDTH, HEIGHT);

    // 注册窗口大小变化时的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // 清空颜色缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换缓冲并检查事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源并退出
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}