#pragma execution_character_set("utf-8")

#include <iostream>
#include <sstream>
#include <map>

#include "Common/Log/LogSystem.h"
#include "Common/Serialize/SerializeSystem.h"
#include "Common/Memory/MemCtrlSystem.h"

#include "Third/rapidjson/document.h"
#include "Third/rapidjson/writer.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"
#include "Third/box2d/box2d.h"
#include "Third/glad/include/glad.h"
#include "Third/glfw/include/glfw3.h"

class SubTest : public Mrk::ISerializable
{
	MRK_SERIALIZABLE(SubTest)
public:
	virtual inline void DeSerialize(const Json::Value& jobj)
	{
		MRK_DESERIALIZE_FIELD(field1, field1);
		MRK_DESERIALIZE_FIELD(field2, field2);
		MRK_DESERIALIZE_FIELD(field3, field3);
		MRK_DESERIALIZE_FIELD(field4, field4);
		MRK_DESERIALIZE_FIELD(field5, field5);
		MRK_DESERIALIZE_FIELD(field6, field6);
	}

	virtual inline void Serialize(Json::Value& jobj, Mrk::JsonAlloc& jalloc) const
	{
		MRK_SERIALIZE_CLASS(SubTest);

		MRK_SERIALIZE_FIELD(field1);
		MRK_SERIALIZE_FIELD(field2);
		MRK_SERIALIZE_FIELD(field3);
		MRK_SERIALIZE_FIELD(field4);
		MRK_SERIALIZE_FIELD(field5);
		MRK_SERIALIZE_FIELD(field6);
	}

public:
	int field1 = 0;
	float field2 = 0;
	double field3 = 0;
	std::string field4 = "NULL";
	std::vector<int> field5 = { 2, 5, 6, 8, 10 };
	bool field6 = true;
};

class Test : public Mrk::ISerializable
{
	MRK_SERIALIZABLE(Test)
public:
	virtual inline void DeSerialize(const Json::Value& jobj)
	{
		MRK_DESERIALIZE_FIELD(field1, field1);
		MRK_DESERIALIZE_FIELD(field2, field2);
		MRK_DESERIALIZE_FIELD(field3, field3);
		MRK_DESERIALIZE_FIELD(field4, field4);
		MRK_DESERIALIZE_FIELD(field5, field5);
		MRK_DESERIALIZE_FIELD(field6, field6);
		MRK_DESERIALIZE_FIELD(field7, field7);
	}

	virtual inline void Serialize(Json::Value& jobj, Mrk::JsonAlloc& jalloc) const
	{
		MRK_SERIALIZE_CLASS(Test);

		MRK_SERIALIZE_FIELD(field1);
		MRK_SERIALIZE_FIELD(field2);
		MRK_SERIALIZE_FIELD(field3);
		MRK_SERIALIZE_FIELD(field4);
		MRK_SERIALIZE_FIELD(field5);
		MRK_SERIALIZE_FIELD(field6);
		MRK_SERIALIZE_FIELD(field7);
	}

public:
	int field1 = 0;
	float field2 = 0;
	double field3 = 0;
	std::string field4 = "NULL";
	std::map<std::string, std::shared_ptr<SubTest>> field5{ {"Serializable", std::make_shared<SubTest>()}, {"2", std::make_shared<SubTest>()}, {"114514", std::make_shared<SubTest>()}, {"序列化测试", std::make_shared<SubTest>()} };
	std::vector<int> field6 = { 1, 3, 5, 7, 9 };
	bool field7 = true;
};

class Test1 : public Test
{
	MRK_SERIALIZABLE(Test1)
public:
	virtual inline void DeSerialize(const Json::Value& jobj)
	{
		MRK_DESERIALIZE_BASED(Test);

		Mrk::JsonValueExtractor::Extract(jobj.FindMember("testfield1")->value, testfield1);;
		Mrk::JsonValueExtractor::Extract(jobj.FindMember("testfield2")->value, testfield2);;
		Mrk::JsonValueExtractor::Extract(jobj.FindMember("testfield3")->value, testfield3);;
	}

	virtual inline void Serialize(Json::Value& jobj, Mrk::JsonAlloc& jalloc) const
	{
		Test::Serialize(jobj, jalloc);
		MRK_SERIALIZE_CLASS(Test1);

		jobj.AddMember(Json::Value("testfield1", jalloc), Mrk::JsonValueCreator::Create(testfield1, jalloc), jalloc);
		jobj.AddMember(Json::Value("testfield2", jalloc), Mrk::JsonValueCreator::Create(testfield2, jalloc), jalloc);
		jobj.AddMember(Json::Value("testfield3", jalloc), Mrk::JsonValueCreator::Create(testfield3, jalloc), jalloc);
	}

public:
	int testfield1 = 0;
	float testfield2 = 0;
	double testfield3 = 0;
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
	test();

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