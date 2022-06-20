#ifndef CJSON_WRAPPER_H
#define CJSON_WRAPPER_H
#include "cjson/cJSON.h"

#include <string>
#include <vector>
#include <initializer_list>
#include <functional>
#include <cassert>
#include <ostream>

class JsonValue;
class JsonArray;
class JsonObject;
class JsonDocument;
class JsonValueRef;

class JsonValue
{
public:
    JsonValue();
    JsonValue(double val);
    JsonValue(bool val);
    JsonValue(int32_t val);
    JsonValue(int64_t val);
    JsonValue(const char *val);
    JsonValue(const std::string &val);
    JsonValue(const JsonObject &val);
    JsonValue(const JsonArray &val);
    JsonValue(const JsonValue &other);
    JsonValue(JsonValue &&other);
    ~JsonValue();

    bool isNull() const {return cJSON_IsNull(item_);}
    bool isBool() const {return cJSON_IsBool(item_);}
    bool isNumber() const {return cJSON_IsNumber(item_);}
    bool isDouble() const {return isNumber();}
    bool isString() const {return cJSON_IsString(item_);}
    bool isArray() const {return cJSON_IsArray(item_);}
    bool isObject() const {return cJSON_IsObject(item_);}
    //cJSON_IsInvalid 在 item_ 为NULL的时候返回false ,按照函数的定义，这里item_为NULL的时候返回true
    bool isUndefined() const {return item_ == nullptr || cJSON_IsInvalid(item_);}

    bool toBool(bool defaultValue = false) const;
    double toNumber(double defaultValue = 0) const;
    double toDouble(double defaultValue = 0) const {return toNumber(defaultValue);}
    int32_t toInt(int32_t defaultValue = 0) const;
    std::string toString() const;
    std::string toString(const std::string &defaultValue) const;
    JsonArray toArray() const;
    JsonArray toArray(const JsonArray &defaultValue) const;
    JsonObject toObject() const;
    JsonObject toObject(const JsonObject &defaultValue) const;

    bool operator == (const JsonValue &other) const;
    bool operator != (const JsonValue &other) const {return !(*this == other);}
    JsonValue &operator = (const JsonValue &other);
    JsonValue &operator = (JsonValue &&other);

private:
    // 内部使用的构造函数
    JsonValue(struct cJSON *item);

    friend class JsonArray;
    friend class JsonObject;
    friend class JsonValueRef;

    struct cJSON *item_;
};

// 内部被JsonObject 和 JsonArray 使用的帮助类
class JsonValueRef
{
public:
    JsonValueRef(struct cJSON *parentItem, struct cJSON *item)
        : parentItem_(parentItem)
        , item_(item)
    {
        assert(parentItem_ && item_);
        assert(parentItem_ != item_);
        assert((cJSON_IsObject(parentItem_) && item_->string)
               || cJSON_IsArray(parentItem));
    }
    JsonValueRef(const JsonValueRef &) = default;
    JsonValueRef &operator = (const JsonValue &other);
    JsonValueRef &operator = (JsonValue &&other);

    operator JsonValue() const {return toValue();}
    JsonValueRef &operator = (const JsonValueRef &val);

    bool isNull() const {return cJSON_IsNull(item_);}
    bool isBool() const {return cJSON_IsBool(item_);}
    bool isNumber() const {return cJSON_IsNumber(item_);}
    bool isDouble() const {return isNumber();}
    bool isString() const {return cJSON_IsString(item_);}
    bool isArray() const {return cJSON_IsArray(item_);}
    bool isObject() const {return cJSON_IsObject(item_);}
    //cJSON_IsInvalid 在 item_ 为NULL的时候返回false ,按照函数的定义，这里item_为NULL的时候返回true
    bool isUndefined() const
    {
        return (parentItem_ == nullptr || item_ == nullptr) ||
                (cJSON_IsInvalid(parentItem_) || cJSON_IsInvalid(item_));
    }

    bool toBool() const {return toValue().toBool();}
    int toInt() const {return toValue().toInt();}
    double toDouble() const {return toValue().toDouble();}
    std::string toString() const {return toValue().toString();}
    JsonArray toArray() const;
    JsonObject toObject() const;

    bool toBool(bool defaultValue) const {return toValue().toBool(defaultValue);}
    int toInt(int defaultValue) const {return toValue().toInt(defaultValue);}
    double toDouble(double defaultValue) const {return toValue().toDouble(defaultValue);}
    std::string toString(const std::string &defaultValue) const {return toValue().toString(defaultValue);}

    bool operator == (const JsonValue &other) const {return toValue() == other;}
    bool operator != (const JsonValue &other) const {return toValue() != other;}

    //Qt没有类似的接口，这里的几个接口是扩展接口
    // 扩展接口 --------------[BEGIN] ---------------
    //如果当前不是JsonObject或者key不存在，函数内部会自动创建和销毁相关对象
    JsonValueRef &operator [] (const std::string &key);
    //如果当前不是JsonObject或者key不存在，则函数的处理是未定义的
    const JsonValueRef &operator [] (const std::string &key) const;

    // 如果index非法或者对象不是JsonArray ,函数的处理是未定义的
    JsonValueRef &operator [] (int index);
    const JsonValueRef &operator [] (int index) const;

    //下面的函数是数组相关的函数，如果不是数组，则函数的处理未定义
    // 添加数组元素,如果当前对象不是数组,函数是未定义的
    void append(const JsonValue &val);
    void append(JsonValue &&val);
    JsonValue last() const;
    JsonValue first() const;
    void removeAt(int index);
    void removeFirst();
    void removeLast();
    JsonValue takeAt(int index);
    int size() const;
    int count() const {return size();}
    bool isEmpty() const {return size() == 0;}

    bool contains(const JsonValue &val) const;
    bool contains(const std::string &key) const;
    // 扩展接口 --------------[END] ---------------

private:
    JsonValue toValue() const;

    mutable struct cJSON *parentItem_;
    mutable struct cJSON *item_;
};

class JsonArray
{
public:
    JsonArray();
    JsonArray(const JsonArray &val);
    JsonArray(JsonArray &&val);
    JsonArray(std::initializer_list<JsonValue> args);
    ~JsonArray();

    void append(const JsonValue &val);
    void append(JsonValue &&val);
    JsonValue at(int index) const;
    int size() const {return cJSON_GetArraySize(item_);}
    int count() const {return size();}
    bool isEmpty() const {return size() == 0;}
    bool contains(const JsonValue &val) const;
    JsonValue last() const;
    JsonValue first() const;
    void removeAt(int index);
    void removeFirst();
    void removeLast();
    JsonValue takeAt(int index);
    void replace(int index, const JsonValue &val);

    JsonValueRef operator [] (int index);
    const JsonValueRef operator [] (int index) const;
    bool operator == (const JsonArray &other) const;
    bool operator != (const JsonArray &other) const {return !(*this == other);}
    JsonArray &operator = (const JsonArray &other);
    JsonArray &operator = (JsonArray &&other);

private:
    // 内部使用，主要为了避免不必要的内存创建和释放
    JsonArray(struct cJSON *item);

    friend class JsonValue;
    friend class JsonDocument;
    friend class JsonValueRef;

    struct cJSON *item_;
};

class JsonObject
{
public:
    JsonObject();
    JsonObject(const JsonObject &other);
    JsonObject(JsonObject &&other);
    JsonObject(std::initializer_list<std::pair<std::string, JsonValue> > args);
    ~JsonObject();

    void insert(const std::string &key, const JsonValue &val);
    void insert(const std::string &key, JsonValue &&val);

    // 键值对的个数
    int size() const {return cJSON_GetArraySize(item_);}
    int count() const {return size();}
    bool isEmpty() const {return size() == 0;}
    std::vector<std::string> keys() const;
    JsonValue value(const std::string &key) const;
    bool contains(const std::string &key) const {return cJSON_HasObjectItem(item_, key.c_str());}
    void remove(const std::string &key) {cJSON_DeleteItemFromObject(item_, key.c_str());}

    bool operator != (const JsonObject &other) const {return !(*this == other);}
    bool operator == (const JsonObject &other) const;
    JsonObject &operator = (const JsonObject &other);
    JsonObject &operator = (JsonObject &&other);
    const JsonValueRef operator [] (const std::string &key) const;
    JsonValueRef operator [] (const std::string &key);

private:
    // 内部使用
    JsonObject(struct cJSON *item);

    friend class JsonValue;
    friend class JsonDocument;
    friend class JsonValueRef;

    struct cJSON *item_;
};

class JsonDocument
{
public:
    enum JsonFormat {
        Indented,
        Compact
    };

    JsonDocument();
    explicit JsonDocument(const JsonObject &object);
    explicit JsonDocument(const JsonArray &array);
    ~JsonDocument();

    JsonDocument(const JsonDocument &other);
    JsonDocument(JsonDocument &&other);
    JsonDocument &operator = (const JsonDocument &other);
    JsonDocument &operator = (JsonDocument &&other);
    bool operator != (const JsonDocument &other) const {return !(*this == other);}
    bool operator == (const JsonDocument &other) const;

    const JsonValue operator [] (const std::string &key) const;
    const JsonValue operator [] (int index) const;

    void swap(JsonDocument &other) {std::swap(item_, other.item_);}

    bool isNull() const {return item_ == nullptr;}

    bool isArray() const {return cJSON_IsArray(item_);}
    JsonArray array() const;
    bool isObject() const {return cJSON_IsObject(item_);}
    JsonObject object() const;
    std::string toJson(JsonFormat format= Indented) const;
    void setArray(const JsonArray &array);
    void setObject(const JsonObject &object);

    static JsonDocument fromJson(const std::string &data, bool *ok = nullptr);

private:
    struct cJSON *item_;
};

std::ostream &operator << (std::ostream &os, const JsonValue &val);

#endif // CJSON_WRAPPER_H
