#pragma execution_character_set("utf-8")

#include <iostream>
#include <sstream>
#include <map>

#include "Marika2d/Common/Log/LogSystem.h"
#include "Marika2d/Common/Serialize/SerializeSystem.h"

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
	std::map<std::string, std::shared_ptr<SubTest>> field5{ {"Serializable", std::make_shared<SubTest>()}, {"2", std::make_shared<SubTest>()}, {"114514", std::make_shared<SubTest>()}, {"���л�����", std::make_shared<SubTest>()} };
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

		MRK_DESERIALIZE_FIELD(testfield1, testfield1);
		MRK_DESERIALIZE_FIELD(testfield2, testfield2);
		MRK_DESERIALIZE_FIELD(testfield3, testfield3);
	}

	virtual inline void Serialize(Json::Value& jobj, Mrk::JsonAlloc& jalloc) const
	{
		MRK_SERIALIZE_BASED(Test);
		MRK_SERIALIZE_CLASS(Test1);

		MRK_SERIALIZE_FIELD(testfield1);
		MRK_SERIALIZE_FIELD(testfield2);
		MRK_SERIALIZE_FIELD(testfield3);
	}

public:
	int testfield1 = 0;
	float testfield2 = 0;
	double testfield3 = 0;
};

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// ���ڴ�С�仯ʱ�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	// ��ʼ�� GLFW
	if (!glfwInit()) {
		std::cerr << "�޷���ʼ�� GLFW" << std::endl;
		return -1;
	}

	// ���� GLFW ʹ�õ� OpenGL �汾
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ���� GLFW ����
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW + GLAD ����", nullptr, nullptr);
	if (!window) {
		std::cerr << "�޷����� GLFW ����" << std::endl;
		glfwTerminate();
		return -1;
	}

	// ���õ�ǰ������
	glfwMakeContextCurrent(window);

	// ��ʼ�� GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "�޷���ʼ�� GLAD" << std::endl;
		return -1;
	}

	// �����ӿ�
	glViewport(0, 0, WIDTH, HEIGHT);

	// ע�ᴰ�ڴ�С�仯ʱ�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		// ��������
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		// �����ɫ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// �������岢����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// �ͷ���Դ���˳�
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}