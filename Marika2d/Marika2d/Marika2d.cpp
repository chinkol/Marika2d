
#include <iostream>
#include <sstream>

#include "Third/box2d/include/box2d.h"
#include "Third/glad/include/glad.h"
#include "Third/glfw/include/glfw3.h"

#include "Marika2d/Common/Log/LogSystem.h"
#include "Marika2d/Common/Serialize/SerializeSystem.h"

#include "Third/rapidjson/document.h"
#include "Third/rapidjson/writer.h"
#include "Third/rapidjson/prettywriter.h"
#include "Third/rapidjson/stringbuffer.h"

#include "Serializer.h"

class BaseClass
{
public:
    virtual inline void VirFunc()
    {
        std::cout << "BaseClass::VirFunc" << "\n";
    }
};

class SubTestClass : public ISerializable
{
    //MRK_SERIALIZABLE(SubTestClass)
public:
    virtual inline void Serialize(SerializeObject* obj) const
    {
        obj->AddMember("subField1", new SerializeNumber(subField1));
        obj->AddMember("subField2", new SerializeNumber(subField2));
        obj->AddMember("subField3", new SerializeNumber(subField3));
        obj->AddMember("subField4", new SerializeString(subField4));
    }
    virtual inline void DeSerialize(SerializeObject* obj)
    {
        SerializeCast::To<SerializeNumber>(obj->GetMember("subField1"))->ReadNum(subField1);
        SerializeCast::To<SerializeNumber>(obj->GetMember("subField2"))->ReadNum(subField2);
        SerializeCast::To<SerializeNumber>(obj->GetMember("subField3"))->ReadNum(subField3);
        SerializeCast::To<SerializeString>(obj->GetMember("subField4"))->ReadStr(subField4);
    }

public:
    int subField1 = 1;
    float subField2 = 3;
    double subField3 = 5;
    std::string subField4 = "SubTest";
};

class TestClass : public BaseClass, public ISerializable
{
    //MRK_SERIALIZABLE(TestClass)
public:
    virtual inline void Serialize(SerializeObject* obj) const
    {
        obj->AddMember("field1", new SerializeNumber(field1));
        obj->AddMember("field2", new SerializeNumber(field2));
        obj->AddMember("field3", new SerializeNumber(field3));
        obj->AddMember("field4", new SerializeString(field4));
        obj->AddMember("field5", new SerializeObject(field5));

        auto arr = new SerializeArray();
        for (auto elem : field6)
        {
            arr->Append(new SerializeNumber(elem));
        }
        obj->AddMember("field6", arr);
    }
    virtual inline void DeSerialize(SerializeObject* obj)
    {
        SerializeCast::To<SerializeNumber>(obj->GetMember("field1"))->ReadNum(field1);
        SerializeCast::To<SerializeNumber>(obj->GetMember("field2"))->ReadNum(field2);
        SerializeCast::To<SerializeNumber>(obj->GetMember("field3"))->ReadNum(field3);
        SerializeCast::To<SerializeString>(obj->GetMember("field4"))->ReadStr(field4);
        SerializeCast::To<SerializeObject>(obj->GetMember("field5"))->ReadObj(field5);

        field6.clear();
        auto arr = SerializeCast::To<SerializeArray>(obj->GetMember("field6"));
        for (size_t i = 0; i < arr->Count(); i++)
        {
            int elem;
            SerializeCast::To<SerializeNumber>(arr->At(i))->ReadNum(elem);
            field6.push_back(elem);
        }
    }

public:
    virtual inline void VirFunc()
    {
        std::cout << "TestClass::VirFunc" << "\n";
    }

public:
    int field1 = 2;
    float field2 = 4;
    double field3 = 6;
    std::string field4 = "Test";
    SubTestClass* field5 = new SubTestClass();
    std::vector<int> field6 = { 1, 2, 3, 4, 5 };
};

int main() 
{
    TestClass* test = new TestClass();
    test->field1 = 0;
    test->field2 = 0;
    test->field3 = 0;
    test->field4 = "NULL";
    test->field5->subField1 = 0;
    test->field5->subField2 = 0;
    test->field5->subField3 = 0;
    test->field5->subField4 = "NULL";
    test->field6 = { 2, 4, 6, 8, 10 };

    TestClass* newTest = new TestClass();
    SerializeObject* obj = new SerializeObject();
    obj->WriteObj(newTest);

    test->DeSerialize(obj);

    return 0;
}