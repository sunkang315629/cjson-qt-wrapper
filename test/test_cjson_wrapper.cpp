#include <gtest/gtest.h>
#include <climits>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

#include "cjson_wrapper.h"

#define CJSON_WRAPPER_EXTEND

TEST(cjson_wrapper, test_json_read_write)
{
    {
        ASSERT_TRUE(JsonValue().isNull());
        ASSERT_TRUE(JsonValue(true).isBool());
        ASSERT_FALSE(JsonValue(false).toBool());
        ASSERT_TRUE(JsonValue(3.14).isNumber());
        ASSERT_TRUE(JsonValue(1024).isDouble());
        ASSERT_TRUE(JsonValue(0).toBool(true));
        ASSERT_FALSE(JsonValue(1024).toBool(false));
        ASSERT_TRUE(JsonValue(1024).toString("hello") == "hello");
        ASSERT_TRUE(JsonValue(false).toString("hello") == "hello");
        ASSERT_TRUE(JsonValue(3.14).toString("hello") == "hello");

        ASSERT_TRUE(JsonValue() == JsonValue());
        ASSERT_FALSE(JsonValue() != JsonValue());

        ASSERT_TRUE(JsonArray() == JsonArray());
        ASSERT_FALSE(JsonArray() != JsonArray());

        ASSERT_TRUE(JsonObject() == JsonObject());
        ASSERT_FALSE(JsonObject() != JsonObject());

        ASSERT_TRUE(JsonValue("hello").toString() == "hello");
        ASSERT_TRUE(JsonValue("test string").isString());
        ASSERT_TRUE(JsonValue("测试文本").toString() == "测试文本");
        ASSERT_TRUE(JsonValue(JsonObject()).isObject());
        ASSERT_TRUE(JsonValue(JsonArray()).isArray());

        ASSERT_TRUE(JsonDocument() == JsonDocument());
        ASSERT_FALSE(JsonDocument() != JsonDocument());

        ASSERT_TRUE(JsonDocument(JsonObject()).object() == JsonObject());
        ASSERT_TRUE(JsonDocument(JsonArray()).array() == JsonArray());
        ASSERT_FALSE(JsonDocument().isObject());
        ASSERT_FALSE(JsonDocument().isArray());

        ASSERT_TRUE(JsonObject().count() == 0);
        ASSERT_TRUE(JsonArray().size() == 0);
        JsonObject rootObject({{"id",1024}});
        ASSERT_TRUE(JsonValue().toObject(rootObject) == rootObject);
        ASSERT_TRUE(JsonValue().toString("hello") == "hello");
        ASSERT_TRUE(JsonValue().toArray(JsonArray({1,2,3,4,5})) == JsonArray({1,2,3,4,5}));
        ASSERT_TRUE(JsonValue().toBool(true));
        ASSERT_FALSE(JsonValue().toBool(false));

        const int32_t maxInt = static_cast<uint32_t>(-1) >> 1;
        const int32_t minInt = -maxInt - 1;
        ASSERT_TRUE(INT_MAX == maxInt);
        ASSERT_TRUE(INT_MIN == minInt);
        ASSERT_TRUE(JsonValue(maxInt).toInt() == maxInt);
    }

    {
        JsonObject rootObject;
        rootObject.insert("id", "1024");
        rootObject["apiVersion"] = "1.0.0";
        rootObject["info"] = "hello world";

        JsonArray numberArry;
        for (int index = 0; index < 1000; ++index) {
            numberArry.append(index + 1);
        }
        numberArry[0] = 112233;
        rootObject.insert("numberArry", numberArry);
        rootObject.remove("numberArry");
        rootObject.insert("numberArry", numberArry);
        ASSERT_TRUE(rootObject.value("numberArry").toArray().size() == 1000);
        rootObject["numberArry"] = JsonArray({1,2,3,4,5,6,7,8,9,10});
        ASSERT_TRUE(rootObject.value("numberArry").toArray().size() == 10);
        for (int index = 1; index <= 10; ++index) {
            ASSERT_TRUE(rootObject.value("numberArry").toArray().contains(index));
        }

        ASSERT_TRUE(rootObject == JsonDocument(rootObject).object());
        const std::string jsonData = JsonDocument(rootObject).toJson(JsonDocument::Compact);
        ASSERT_TRUE(JsonDocument::fromJson(jsonData).object() == rootObject);

        JsonArray objectArry;
        for (int index = 0; index < 1000; ++index) {
            objectArry.append(rootObject);
        }

        ASSERT_TRUE(objectArry.size() == 1000);
        while (!objectArry.isEmpty()) {
            ASSERT_TRUE(objectArry.at(0) == rootObject);
            ASSERT_TRUE(objectArry.takeAt(0).toObject() == rootObject);
        }

        ASSERT_TRUE(objectArry.isEmpty() && JsonValue(objectArry).isArray());
        objectArry.append(rootObject);
        ASSERT_TRUE(objectArry.last() == objectArry.first());
        ASSERT_TRUE(objectArry.contains(rootObject));

        objectArry = JsonArray();
        objectArry.append(rootObject);
        objectArry.append(rootObject);
        objectArry.removeFirst();
        objectArry.removeLast();
        ASSERT_TRUE(objectArry.isEmpty());
    }

    {
        JsonObject testObject;
        testObject["hello world"];
        ASSERT_TRUE(testObject.count() == 1);
        ASSERT_TRUE(testObject["hello world"].isNull());
    }

    {
        ASSERT_TRUE(JsonDocument::fromJson("{}").toJson(JsonDocument::Compact) == "{}");
        ASSERT_TRUE(JsonDocument::fromJson("{}").object().isEmpty());
        ASSERT_TRUE(JsonDocument::fromJson("[]").toJson(JsonDocument::Compact) == "[]");
        ASSERT_TRUE(JsonDocument::fromJson("[]").array().isEmpty());
    }
}

TEST(cjson_wrapper, test_const_object)
{
    {
        JsonObject rootObject;
        rootObject["id"] = "1024";
        rootObject["apiVersion"] = "1.0.0";

        const JsonObject rootObject_1(rootObject);
        ASSERT_TRUE(rootObject_1.count() == 2);
        ASSERT_TRUE(rootObject_1.value("id") == "1024");
        ASSERT_TRUE(rootObject_1.value("apiVersion") == "1.0.0");
        ASSERT_TRUE(!rootObject_1.isEmpty());
        ASSERT_TRUE(rootObject_1["id"] == "1024");
        ASSERT_TRUE(rootObject_1.contains("apiVersion"));
    }

    {
        JsonArray rootArry;
        rootArry.append(JsonObject({{"id", "1024"}}));
        rootArry.append(JsonArray({1,2,3,4,5}));
        rootArry.append(99999);
        rootArry.append("hello world");
        rootArry.append(JsonValue());
        rootArry.append(3.14);

        const JsonArray rootArry_1(rootArry);
        ASSERT_TRUE(rootArry_1.count() == 6);
        ASSERT_TRUE(rootArry_1 == rootArry);
        ASSERT_TRUE(rootArry_1[0].isObject());
        ASSERT_TRUE(rootArry_1[1].isArray());
        ASSERT_TRUE(rootArry_1[1] == JsonArray({1,2,3,4,5}));
        ASSERT_TRUE(rootArry_1[2] == 99999);
        ASSERT_TRUE(rootArry_1[3] == "hello world");
        ASSERT_TRUE(rootArry_1[4].isNull());
        ASSERT_DOUBLE_EQ(rootArry_1[5].toDouble(), 3.14);
    }

}

TEST(cjson_wrapper, test_jsonvalueref)
{
    {
        JsonObject testObject;
        ASSERT_TRUE(testObject["test_null"].isNull()); //这里自动创建了一个空的对象
        ASSERT_TRUE(testObject["test_null"] == JsonValue());
        ASSERT_FALSE(testObject["test_null"] != JsonValue());
        ASSERT_TRUE(testObject.count() == 1);
        testObject["test_null"] = false;
        ASSERT_FALSE(testObject["test_null"].toBool());
        ASSERT_TRUE(testObject["test_null"].isBool());

        testObject["test_1"] = JsonValue("hello world");
        ASSERT_TRUE(testObject["test_1"].isString());
        ASSERT_TRUE(testObject["test_1"].toString() == "hello world");
        ASSERT_TRUE(testObject["test_1"] == JsonValue("hello world"));

        testObject["test_2"] = JsonValue(true);
        ASSERT_TRUE(testObject["test_2"].isBool());
        ASSERT_TRUE(testObject["test_2"].toBool());
        ASSERT_TRUE(testObject["test_2"] == JsonValue(true));

        testObject["test_3"] = JsonValue(false);
        ASSERT_TRUE(testObject["test_3"].isBool());
        ASSERT_FALSE(testObject["test_3"].toBool());
        ASSERT_TRUE(testObject["test_3"] == JsonValue(false));

        testObject["test_4"] = JsonValue(1024);
        ASSERT_TRUE(testObject["test_4"].isNumber());
        ASSERT_TRUE(testObject["test_4"].toInt() == 1024);
        ASSERT_TRUE(testObject["test_4"] == JsonValue(1024));
        testObject["test_4"] = 3.14;
        ASSERT_DOUBLE_EQ(testObject["test_4"].toDouble(), 3.14);


        JsonObject tmpVal{
            {"id", "1024"},
            {"apiVersion", "1.0.0"},
            {"val", 112233}};

        testObject["test_5"] = tmpVal;
        ASSERT_TRUE(testObject["test_5"] == tmpVal);
        ASSERT_TRUE(testObject["test_5"].toObject() == tmpVal);
        ASSERT_TRUE(testObject["test_5"].toArray() == JsonArray());

        testObject["test_5"] = testObject["test_4"];
        ASSERT_DOUBLE_EQ(testObject["test_5"].toDouble(), 3.14);
    }
}

#ifdef CJSON_WRAPPER_EXTEND

TEST(cjson_wrapper, test_json_extra)
{
    {
        JsonObject rootObject;
        ASSERT_TRUE(JsonObject()["111"]["2222"]["333333333"].isNull());
        rootObject["objectInfo"]["test_1"]["info_1"] = false;
        rootObject["objectInfo"]["test_1"]["info_2"] = JsonValue();
        rootObject["objectInfo"]["test_1"]["info_3"] = JsonArray({1,2,3,4,5,6,7,8,9,10});
        rootObject["objectInfo"]["test_1"]["info_3"][9] = 99999;
        ASSERT_TRUE(rootObject["objectInfo"]["test_1"]["info_3"][1] == 2);
        ASSERT_FALSE(rootObject["objectInfo"]["test_1"]["info_1"].toBool());
        rootObject["rootObject"] = rootObject;
        ASSERT_TRUE(rootObject["rootObject"]["objectInfo"]["test_1"]["info_3"][2] == 3);
        rootObject["11"]["22"]["33"]["44"]["55"]["66"]["77"]["88"]["99"]["100"] = "hello world";
        for (int index = 0; index < 100; ++index) {
            ASSERT_TRUE(rootObject["11"]["22"]["33"]["44"]["55"].contains(std::string("66")));
            ASSERT_TRUE(rootObject["11"]["22"]["33"]["44"]["55"]["66"]["77"]["88"]["99"]["100"] == "hello world");
            ASSERT_TRUE(rootObject["11"]["22"]["33"]["44"]["55"]["66"]["77"]["88"]["99"]["999"].isNull());
        }
        cout << JsonDocument(rootObject).toJson() << endl;
        cout << JsonDocument(rootObject).toJson(JsonDocument::Compact) << endl;
    }

    {
        JsonObject rootObject;
        rootObject["index_1"]["index_2"]["index_4"] = JsonArray();
        for (int index = 0; index < 1000; ++index) {
            rootObject["index_1"]["index_2"]["index_4"].append(index + 1);
        }
        ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].count() == 1000);
        ASSERT_EQ(rootObject["index_1"]["index_2"]["index_4"].count(), 1000);
        for (int index = 0; index < 10; ++index) {
            ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].contains(1000));
        }
        ASSERT_TRUE(rootObject.count() == 1);
        rootObject["index_1"]["index_2"]["index_4"].removeFirst();
        ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].first() == 2);
        rootObject["index_1"]["index_2"]["index_4"].removeLast();
        ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].last() == 999);
        ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].count() == 998);
        while (rootObject["index_1"]["index_2"]["index_4"].count()) {
            ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].takeAt(0).isNumber());
        }
        ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"].isEmpty());

        ASSERT_TRUE(rootObject["index_1"] == rootObject["index_1"]);
        ASSERT_TRUE(rootObject["index_1"]["index_2"] == rootObject["index_1"]["index_2"]);
        ASSERT_TRUE(rootObject["index_1"]["index_2"]["index_4"] == rootObject["index_1"]["index_2"]["index_4"]);

        const JsonObject rootObject_const(rootObject);
        ASSERT_TRUE(rootObject_const["index_1"]["index_2"]["index_4"] == rootObject_const["index_1"]["index_2"]["index_4"]);
        ASSERT_TRUE(rootObject_const["index_1"]["index_2"]["index_4"].isEmpty());
        ASSERT_TRUE(rootObject_const == rootObject);
    }

    {
        JsonArray rootArry;
        rootArry.append(1024);
        rootArry[0]["11"]["22"]["33"] = "hello world";
        ASSERT_TRUE(rootArry[0]["11"]["22"]["33"] == "hello world");
        ASSERT_TRUE(rootArry[0]["11"]["22"]["33"].isString());
        ASSERT_TRUE(rootArry[0].isObject());
        ASSERT_TRUE(rootArry[0].count() == 1);
    }

    {
        JsonObject rootObject;
        rootObject["id"] = "1024";
        rootObject["data"]["index_1"]["val_1"] = true;
        rootObject["data"]["index_2"]["val_1"] = "hello world";
        rootObject["data"]["index_3"]["val_1"] = 3.14;

        const JsonObject rootObject_1(rootObject);
        ASSERT_TRUE(rootObject_1["id"] == "1024");
        ASSERT_TRUE(rootObject_1["data"]["index_1"]["val_1"].isBool());
        ASSERT_TRUE(rootObject_1["data"]["index_3"]["val_1"].isNumber());
        ASSERT_DOUBLE_EQ(rootObject_1["data"]["index_3"]["val_1"].toDouble(), 3.14);
        ASSERT_TRUE(rootObject["data"]["index_2"].count() == 1);
    }
}

TEST(cjson_wrapper, string_write)
{
    {
        std::stringstream str_stream;
        str_stream << JsonValue("hello world");
        ASSERT_TRUE(str_stream.str() == "hello world");
    }

    {
        std::stringstream str_stream;
        str_stream << JsonValue();
        ASSERT_TRUE(str_stream.str() == "null");
    }

    {
        std::stringstream str_stream;
        str_stream << JsonValue(true);
        ASSERT_TRUE(str_stream.str() == "true");
    }

    {
        std::stringstream str_stream;
        str_stream << JsonValue(false);
        ASSERT_TRUE(str_stream.str() == "false");
    }

    {
        std::stringstream str_stream;
        str_stream << JsonValue(3.14);
        ASSERT_TRUE(str_stream.str() == "3.14");
    }

    {
        std::stringstream str_stream;
        JsonObject testObject;
        testObject["id"] = "1024";
        testObject["index_1"]["index_2"]["index_3"] = 3.141592654;
        str_stream << testObject;
        ASSERT_TRUE(str_stream.str() == JsonDocument(testObject).toJson(JsonDocument::Compact));
    }

    {
        std::stringstream str_stream;
        JsonArray testArry;
        testArry.append("hello world");
        for (int index = 0; index < 100; ++index) {
            testArry.append(index + 1);
        }
        str_stream << testArry;
        ASSERT_TRUE(str_stream.str() == JsonDocument(testArry).toJson(JsonDocument::Compact));
    }
}

#endif

TEST(cjson_wrapper, benchmark_test_cjson_wrapper)
{
    std::string jsonData;
    {
        JsonObject rootObject;
        JsonObject tmpObject({
                                 {"index_1","1"},
                                 {"index_2","2"},
                                 {"index_3","3"},
                                 {"index_4","4"},
                                 {"index_5","5"},
                             });

        JsonArray tmpArray;

        for (int index = 0; index < 1000; ++index) {
            tmpArray.append(tmpObject);
        }
        rootObject["objectArray"] = tmpArray;
        rootObject["id"] = "1";
        jsonData = JsonDocument(rootObject).toJson(JsonDocument::Compact);
        cout << "json data length: .........." << JsonDocument(rootObject).toJson(JsonDocument::Compact).length() / 1024.0f / 1024.0f << endl;
    }

    {
        for (int index = 0; index < 10; ++index) {
            bool ok;
            JsonDocument document(JsonDocument::fromJson(jsonData, &ok));
            ASSERT_TRUE(document.toJson(JsonDocument::Compact).length() == jsonData.length());
            ASSERT_TRUE(ok);
            const JsonObject rootObject(document.object());
            ASSERT_TRUE(rootObject["id"].toString() == "1");
            ASSERT_TRUE(rootObject["objectArray"].toArray().size() == 1000);
        }
    }

    {
        JsonObject rootObject;
        rootObject["data"]["index_1"]["index_2"] = "hello world";

        const JsonDocument document(rootObject);
        ASSERT_TRUE(document.object() == rootObject);
        ASSERT_TRUE(document.isObject());
        ASSERT_TRUE(document.object()["data"]["index_1"]["index_2"] == "hello world");
        ASSERT_TRUE(document == JsonDocument(rootObject));
        ASSERT_FALSE(document != JsonDocument(rootObject));

        const std::string jsonString(JsonDocument(rootObject).toJson(JsonDocument::Compact));
        ASSERT_TRUE(JsonDocument::fromJson(jsonString).object()["data"]["index_1"]["index_2"] == "hello world");

        JsonDocument document_1;
        document_1.setObject(rootObject);
        ASSERT_TRUE(document_1 == document);
        ASSERT_TRUE(document_1.object() == document.object());
    }

    {
        JsonArray rootArry;
        for (int index = 0; index < 100; ++index) {
            rootArry.append(index + 1);
        }

        JsonDocument document;
        document.setArray(rootArry);
        ASSERT_TRUE(document.array() == rootArry);
    }
}


