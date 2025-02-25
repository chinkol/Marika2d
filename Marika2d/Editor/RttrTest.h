#pragma once

#include "Core/Component/Component.h"
#include "Core/GameObject/GameObject.h"

namespace MrkTest
{
	class BoneComponent : public Mrk::Component
	{
		MRK_COMPONENT(BoneComponent)
	public:
		double inline GetData1() { return data1; }
		void inline SetData1(double val) { data1 = val; }
	public:
		std::string data2 = "Hello";
		int data0 = 1209;
		std::vector<std::string> data3 = { "H", "E", "L", "L", "O" };
		std::map<std::string, double> data4 = { {"H", 1}, {"E", 2}, {"L", 3}, {"L", 4}, {"O", 5} };
	private:
		double data1 = 12.09;
	};

	class Bone : public Mrk::GameObject
	{
		MRK_GAMEOBJECT(Bone)
	public:
	};
}

namespace MrkTest
{
	void inline Test()
	{
		auto bone1 = Mrk::GameObjectFactory::CreateNew<Bone>();
		auto bone11 = Mrk::GameObjectFactory::CreateNew<Bone>();
		auto bone12 = Mrk::GameObjectFactory::CreateNew<Bone>();
		auto bone211 = Mrk::GameObjectFactory::CreateNew<Bone>();
		auto bone212 = Mrk::GameObjectFactory::CreateNew<Bone>();
		auto bone221 = Mrk::GameObjectFactory::CreateNew<Bone>();
		auto bone222 = Mrk::GameObjectFactory::CreateNew<Bone>();

		bone1->AddComponent<BoneComponent>();
		bone11->AddComponent<BoneComponent>();
		bone12->AddComponent<BoneComponent>();
		bone211->AddComponent<BoneComponent>();
		bone212->AddComponent<BoneComponent>();
		bone221->AddComponent<BoneComponent>();
		bone222->AddComponent<BoneComponent>();

		bone1->AddChild(bone11);
		bone1->AddChild(bone12);
		bone11->AddChild(bone211);
		bone11->AddChild(bone212);
		bone12->AddChild(bone221);
		bone12->AddChild(bone222);

		Json::Document jdoc(Json::ArrayType);
		auto json = bone1->ToJson(jdoc.GetAllocator());

		Json::StringBuffer buf;
		Json::PrettyWriter writer(buf);

		json.Accept(writer);

		std::cout << buf.GetString();

		auto db = Mrk::GameObjectFactory::CreateNew<Bone>();
		db->FromJson(json);


		Json::StringBuffer buf1;
		Json::PrettyWriter writer1(buf1);

		auto json1 = db->ToJson(jdoc.GetAllocator());

		json1.Accept(writer1);

		std::cout << buf1.GetString();
	}
}