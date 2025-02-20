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
		std::shared_ptr<Bone> bone1;

		std::shared_ptr<Bone> bone11;										std::shared_ptr<Bone> bone12;

		std::shared_ptr<Bone> bone211; std::shared_ptr<Bone> bone212;		std::shared_ptr<Bone> bone221; std::shared_ptr<Bone> bone222;

		bone1 = std::make_shared<Bone>(); bone1->SetName("Bone1");
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone1);
		bone11 = std::make_shared<Bone>();
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone11);
		bone12 = std::make_shared<Bone>();
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone12);
		bone211 = std::make_shared<Bone>();
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone211);
		bone212 = std::make_shared<Bone>();
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone212);
		bone221 = std::make_shared<Bone>();
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone221);
		bone222 = std::make_shared<Bone>();
		Mrk::GameObjectOperate::AttachComponent<BoneComponent>(bone222);

		Mrk::GameObjectOperate::AttachChild(bone11, bone1);
		Mrk::GameObjectOperate::AttachChild(bone12, bone1);

		Mrk::GameObjectOperate::AttachChild(bone211, bone11);
		Mrk::GameObjectOperate::AttachChild(bone212, bone11);

		Mrk::GameObjectOperate::AttachChild(bone221, bone12);
		Mrk::GameObjectOperate::AttachChild(bone222, bone12);

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