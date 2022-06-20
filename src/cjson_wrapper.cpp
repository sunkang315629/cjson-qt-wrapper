#include "cjson_wrapper.h"

#include <utility>

//------------------[JsonValue] BEGIN---------------------

JsonValue::JsonValue()
    : item_(cJSON_CreateNull())
{
    assert(item_ != nullptr);
}

JsonValue::JsonValue(double val)
    : item_(cJSON_CreateNumber(val))
{
    assert(item_ != nullptr);
}

JsonValue::JsonValue(int32_t val)
    : JsonValue(static_cast<double>(val))
{

}

JsonValue::JsonValue(int64_t val)
    : JsonValue(static_cast<double>(val))
{

}

JsonValue::JsonValue(const char *val)
    : JsonValue(std::string(val))
{

}

JsonValue::JsonValue(bool val)
    : item_(cJSON_CreateBool(val))
{
    assert(item_ != nullptr);
}

JsonValue::JsonValue(const std::string &val)
    : item_(cJSON_CreateString(val.c_str()))
{
    assert(item_ != nullptr);
}

JsonValue::JsonValue(const JsonObject &val)
    : item_(nullptr)
{
    item_ = cJSON_Duplicate(val.item_, 1);
    assert(item_ != nullptr);
}

JsonValue::JsonValue(const JsonArray &val)
    : item_(nullptr)
{
    item_ = cJSON_Duplicate(val.item_, 1);
    assert(item_ != nullptr);
}

JsonValue::JsonValue(const JsonValue &other)
    : item_(nullptr)
{
    if (other.item_) {
        item_ = cJSON_Duplicate(other.item_, 1);
        assert(item_ != nullptr);
    }
}

JsonValue::JsonValue(JsonValue &&other)
    : item_(nullptr)
{
    std::swap(item_, other.item_);
}

JsonValue::JsonValue(struct cJSON *item)
    : item_(item)
{

}

JsonValue::~JsonValue()
{
    if (item_) {
        cJSON_Delete(item_);
    }
}

bool JsonValue::toBool(bool defaultValue) const
{
    if (!isBool()) {
        return defaultValue;
    }
    return item_->type == cJSON_True;
}

double JsonValue::toNumber(double defaultValue) const
{
    if (!isNumber()) {
        return defaultValue;
    }
    return item_->valuedouble;
}

int32_t JsonValue::toInt(int32_t defaultValue) const
{
    if (!isNumber()) {
        return defaultValue;
    }
    return item_->valueint;
}

std::string JsonValue::toString() const
{
    if (!isString()) {
        return std::string();
    }

    const char *str = cJSON_GetStringValue(item_);
    assert(str != nullptr);
    return str;
}

std::string JsonValue::toString(const std::string &defaultValue) const
{
    if (!isString()) {
        return defaultValue;
    }

    const char *str = cJSON_GetStringValue(item_);
    assert(str != nullptr);
    return str;
}

JsonArray JsonValue::toArray() const
{
    if (!isArray()) {
        return JsonArray(); //空的数组
    }

    return JsonArray(cJSON_Duplicate(item_, 1));
}

JsonArray JsonValue::toArray(const JsonArray &defaultValue) const
{
    if (!isArray()) {
        return defaultValue;
    }

    return JsonArray(cJSON_Duplicate(item_, 1));
}

JsonObject JsonValue::toObject() const
{
    if (!isObject()) {
        return JsonObject(); //空的对象
    }

    return JsonObject(cJSON_Duplicate(item_, 1));
}

JsonObject JsonValue::toObject(const JsonObject &defaultValue) const
{
    if (!isObject()) {
        return defaultValue;
    }

    return JsonObject(cJSON_Duplicate(item_, 1));
}

bool JsonValue::operator == (const JsonValue &other) const
{
    if (this == &other) {
        return true;
    }
    assert(item_ != other.item_); //按照JsonValue的处理逻辑，这里不应该出现相等指针的情况
    return cJSON_Compare(item_, other.item_, 1);
}

JsonValue &JsonValue::operator = (const JsonValue &other)
{
    if (this == &other) {
        return *this;
    }
    assert(other.item_ != nullptr);
    cJSON_Delete(item_);
    item_ = cJSON_Duplicate(other.item_, 1);
    return *this;
}

JsonValue &JsonValue::operator = (JsonValue &&other)
{
    if (this == &other) {
        return *this;
    }

    std::swap(item_, other.item_);
    return *this;
}

//------------------[JsonValue] END---------------------

//------------------[JsonValueRef] BEGIN---------------------

JsonValueRef &JsonValueRef::operator = (const JsonValue &other)
{
    assert(!other.isUndefined());
    if (item_ == other.item_) {
        return *this;
    }

    struct cJSON *newItem = cJSON_Duplicate(other.item_, 1);
    if (cJSON_IsObject(parentItem_)) {
        std::swap(item_->string, newItem->string);
    }
    // 内部会释放item_ 的资源
    bool ret = cJSON_ReplaceItemViaPointer(parentItem_, item_, newItem);
    assert(ret);
    (void)ret;
    item_ = newItem;
    return *this;
}

JsonValueRef &JsonValueRef::operator = (JsonValue &&other)
{
    assert(!other.isUndefined());
    if (item_ == other.item_) {
        return *this;
    }

    if (cJSON_IsObject(parentItem_)) {
        std::swap(item_->string, other.item_->string);
    }
    // 内部会释放item_ 的资源
    bool ret = cJSON_ReplaceItemViaPointer(parentItem_, item_, other.item_);
    assert(ret);
    (void)ret;
    item_ = other.item_;
    other.item_ = nullptr;
    return *this;
}

JsonValueRef &JsonValueRef::operator = (const JsonValueRef &val)
{
    if (this == &val) {
        return *this;
    }

    operator = (val.toValue());
    return *this;
}

JsonArray JsonValueRef::toArray() const
{
    if (!cJSON_IsArray(item_)) {
        return JsonArray();
    }

    cJSON *newItem = cJSON_Duplicate(item_, 1);
    assert(newItem != nullptr);
    return JsonArray(newItem);
}

JsonObject JsonValueRef::toObject() const
{
    if (!cJSON_IsObject(item_)) {
        return JsonObject();
    }

    cJSON *newItem = cJSON_Duplicate(item_, 1);
    assert(newItem != nullptr);
    return JsonObject(newItem);
}

JsonValueRef &JsonValueRef::operator [] (const std::string &key)
{
    assert(parentItem_ != nullptr && item_ != nullptr);
    assert(parentItem_ != item_);
    if (!cJSON_IsObject(item_)) {
        operator = (JsonObject());
        assert(cJSON_IsObject(item_));
    }
    cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
    if (!curItem) {
        curItem = cJSON_CreateNull();
        cJSON_AddItemToObject(item_, key.c_str(), curItem);
    }
    parentItem_ = item_;
    item_ = curItem;
    return *this;
}

const JsonValueRef &JsonValueRef::operator [] (const std::string &key) const
{
    assert(parentItem_ != nullptr && item_ != nullptr);
    assert(parentItem_ != item_);
    if (!cJSON_IsObject(item_)) {
        assert(false);
        return *this;
    }

    cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
    if (!curItem) {
        assert(false);
        return *this;
    }

    parentItem_ = item_;
    item_ = curItem;
    return *this;
}

JsonValueRef &JsonValueRef::operator [] (int index)
{
    assert(parentItem_ != nullptr && item_ != nullptr);
    assert(parentItem_ != item_);
    if (!cJSON_IsArray(item_) || index >= cJSON_GetArraySize(item_)) {
        assert(false);
        return *this;
    }

    cJSON *curItem = cJSON_GetArrayItem(item_, index);
    assert(curItem != nullptr);
    parentItem_ = item_;
    item_ = curItem;
    return *this;
}

const JsonValueRef &JsonValueRef::operator [] (int index) const
{
    return const_cast<JsonValueRef*>(this)->operator [] (index);
}

void JsonValueRef::append(const JsonValue &val)
{
    assert(parentItem_ != nullptr && item_ != nullptr);
    assert(parentItem_ != item_);

    if (!cJSON_IsArray(item_)) {
        assert(false);
        return;
    }

    JsonArray arry(item_);
    arry.append(val);
    arry.item_ = nullptr;
}

void JsonValueRef::append(JsonValue &&val)
{
    assert(parentItem_ != nullptr && item_ != nullptr);
    assert(parentItem_ != item_);

    if (!cJSON_IsArray(item_)) {
        assert(false);
        return;
    }

    JsonArray arry(item_);
    arry.append(std::move(val));
    arry.item_ = nullptr;
}

JsonValue JsonValueRef::last() const
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    JsonValue val(arry.last());
    arry.item_ = nullptr;
    return val;
}

JsonValue JsonValueRef::first() const
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    JsonValue val(arry.first());
    arry.item_ = nullptr;
    return val;
}

void JsonValueRef::removeAt(int index)
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    arry.removeAt(index);
    arry.item_ = nullptr;
}

void JsonValueRef::removeFirst()
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    arry.removeFirst();
    arry.item_ = nullptr;
}

void JsonValueRef::removeLast()
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    arry.removeLast();
    arry.item_ = nullptr;
}

JsonValue JsonValueRef::takeAt(int index)
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    JsonValue val(arry.takeAt(index));
    arry.item_ = nullptr;
    return val;
}

int JsonValueRef::size() const
{
    return cJSON_GetArraySize(item_);
}

bool JsonValueRef::contains(const JsonValue &val) const
{
    assert(cJSON_IsArray(item_));
    JsonArray arry(item_);
    bool ret = arry.contains(val);
    arry.item_ = nullptr;
    return ret;
}

bool JsonValueRef::contains(const std::string &key) const
{
    assert(cJSON_IsObject(item_));
    JsonObject object(item_);
    bool ret = object.contains(key);
    object.item_ = nullptr;
    return ret;
}

JsonValue JsonValueRef::toValue() const
{
    assert(item_ != nullptr);
    cJSON *newItem = cJSON_Duplicate(item_, 1);
    assert(newItem != nullptr);
    return JsonValue(newItem);
}

//------------------[JsonValueRef] END---------------------


//------------------[JsonArray] BEGIN---------------------
JsonArray::JsonArray()
    : item_(cJSON_CreateArray())
{
    assert(item_ != nullptr);
}

JsonArray::JsonArray(const JsonArray &val)
    : item_(nullptr)
{
    assert(cJSON_IsArray(val.item_));
    item_ = cJSON_Duplicate(val.item_, 1);
    assert(item_ != nullptr);
}

JsonArray::JsonArray(JsonArray &&val)
    : item_(nullptr)
{
    assert(cJSON_IsArray(val.item_));
    std::swap(item_, val.item_);
}

JsonArray::JsonArray(std::initializer_list<JsonValue> args)
    : item_(cJSON_CreateArray())
{
    for (const auto &val : args) {
        append(val);
    }
}

JsonArray::JsonArray(struct cJSON *item)
    : item_(item)
{
    assert(item_ != nullptr);
}

JsonArray::~JsonArray()
{
    if (item_) {
        cJSON_Delete(item_);
    }
}

void JsonArray::append(const JsonValue &val)
{
    struct cJSON *tmpItem = cJSON_Duplicate(val.item_, 1);
    assert(tmpItem != nullptr);
    cJSON_AddItemToArray(item_, tmpItem);
}

void JsonArray::append(JsonValue &&val)
{
    cJSON_AddItemToArray(item_, val.item_);
    val.item_ = nullptr;
}

JsonValue JsonArray::at(int index) const
{
    if (index >= size()) {
        return JsonValue(static_cast<struct cJSON*>(nullptr)); //返回一个非法的 JsonValue
    }
    struct cJSON *item = cJSON_GetArrayItem(item_, index);
    assert(item != nullptr);
    // 这里需要把item复制一份，不能直接使用item指针，否则会出现重复释放内存的错误
    struct cJSON *tmpItem = cJSON_Duplicate(item, 1);
    assert(tmpItem != nullptr);
    return JsonValue(tmpItem);
}

bool JsonArray::contains(const JsonValue &val) const
{
    int arryLength = size();
    for (int index = 0; index < arryLength; ++index) {
        if (at(index) == val) {
            return true;
        }
    }
    return false;
}

JsonValue JsonArray::last() const
{
    int arryLength = size();
    if (arryLength == 0) {
        return JsonValue(static_cast<struct cJSON*>(nullptr)); // 返回一个非法值
    }
    assert(arryLength > 0);
    struct cJSON *curItem = cJSON_GetArrayItem(item_, arryLength - 1);
    assert(curItem != nullptr);
    struct cJSON *newItem = cJSON_Duplicate(curItem, 1);
    assert(newItem != nullptr);
    return JsonValue(newItem);
}

JsonValue JsonArray::first() const
{
    int arryLength = size();
    if (arryLength == 0) {
        return JsonValue(static_cast<struct cJSON*>(nullptr)); // 返回一个非法值
    }
    assert(arryLength > 0);
    struct cJSON *curItem = cJSON_GetArrayItem(item_, 0);
    assert(curItem != nullptr);
    struct cJSON *newItem = cJSON_Duplicate(curItem, 1);
    assert(newItem != nullptr);
    return JsonValue(newItem);
}

void JsonArray::removeAt(int index)
{
    int arryLength = size();
    assert(index >= 0 && index < arryLength);
    cJSON_DeleteItemFromArray(item_, index);
}

void JsonArray::removeFirst()
{
    int arryLength = size();
    assert(arryLength > 0);
    removeAt(0);
}

void JsonArray::removeLast()
{
    int arryLength = size();
    assert(arryLength > 0);
    removeAt(arryLength - 1);
}

JsonValue JsonArray::takeAt(int index)
{
    assert(index < size() && index >= 0);
    struct cJSON *curItem = cJSON_DetachItemFromArray(item_, index);
    assert(curItem != nullptr);
    return JsonValue(curItem);
}

void JsonArray::replace(int index, const JsonValue &val)
{
    int arryLength = size();
    assert(index >= 0 && index < arryLength);
    assert(!val.isUndefined());
    operator [] (index) = val;
}

JsonValueRef JsonArray::operator [] (int index)
{
    return static_cast<const JsonArray*>(this)->operator [] (index);
}

const JsonValueRef JsonArray::operator [] (int index) const
{
    int arryLength = size();
    assert(index >= 0 && index < arryLength);
    struct cJSON *curItem = cJSON_GetArrayItem(item_, index);
    assert(curItem != nullptr);
    return JsonValueRef(item_, curItem);
}

bool JsonArray::operator == (const JsonArray &other) const
{
    if (this == &other) {
        return true;
    }
    return cJSON_Compare(item_, other.item_, 1);
}

JsonArray &JsonArray::operator = (const JsonArray &other)
{
    if (this == &other) {
        return *this;
    }

    cJSON_Delete(item_);
    item_ = cJSON_Duplicate(other.item_, 1);
    return *this;
}

JsonArray &JsonArray::operator = (JsonArray &&other)
{
    if (this == &other) {
        return *this;
    }

    std::swap(item_, other.item_);
    return *this;
}

//------------------[JsonArray] END---------------------


//------------------[JsonObject] BEGIN---------------------

JsonObject::JsonObject()
    : item_(cJSON_CreateObject())
{
    assert(item_ != nullptr);
}

JsonObject::JsonObject(const JsonObject &other)
    : item_(nullptr)
{
    assert(cJSON_IsObject(other.item_));
    item_ = cJSON_Duplicate(other.item_, 1);
    assert(item_ != nullptr);
}

JsonObject::JsonObject(JsonObject &&other)
    : item_(nullptr)
{
    assert(cJSON_IsObject(other.item_));
    std::swap(item_, other.item_);
}

JsonObject::JsonObject(std::initializer_list<std::pair<std::string, JsonValue> > args)
    : item_(cJSON_CreateObject())
{
    for (const auto &val : args) {
        insert(val.first, val.second);
    }
}

JsonObject::JsonObject(struct cJSON *item)
    : item_(item)
{
    assert(item_ != nullptr);
}

JsonObject::~JsonObject()
{
    if (item_) {
        cJSON_Delete(item_);
    }
}

void JsonObject::insert(const std::string &key, const JsonValue &val)
{
    assert(!val.isUndefined());
    assert(!key.empty());
    if (!contains(key)) {
        struct cJSON *newItem = cJSON_Duplicate(val.item_, 1);
        cJSON_AddItemToObject(item_, key.c_str(), newItem);
    } else {
        struct cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
        assert(curItem != nullptr);
        assert(std::string(curItem->string) == key);
        struct cJSON *newItem = cJSON_Duplicate(val.item_, 1);
        std::swap(curItem->string, newItem->string);
        // curItem 指向的资源会被销毁
        cJSON_ReplaceItemViaPointer(item_, curItem, newItem);
    }
}

void JsonObject::insert(const std::string &key, JsonValue &&val)
{
    assert(!val.isUndefined());
    assert(!key.empty());
    if (!contains(key)) {
        cJSON_AddItemToObject(item_, key.c_str(), val.item_);
        val.item_ = nullptr; //把 JsonValue 置为非法,可以正常调用析构函数即可
    } else {
        struct cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
        assert(curItem != nullptr);
        assert(std::string(curItem->string) == key);
        std::swap(curItem->string, val.item_->string);
        // curItem 指向的资源会被销毁
        cJSON_ReplaceItemViaPointer(item_, curItem, val.item_);
        val.item_ = nullptr;
    }
}

std::vector<std::string> JsonObject::keys() const
{
    std::vector<std::string> keysData;
    auto currentItem = item_->child;
    while (currentItem) {
        keysData.push_back(currentItem->string);
        currentItem = currentItem->next;
    }
    return keysData;
}

JsonValue JsonObject::value(const std::string &key) const
{
    struct cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
    if (!curItem) {
        return JsonValue(static_cast<struct cJSON*>(nullptr)); // 返回一个非法值
    }

    struct cJSON *newItem = cJSON_Duplicate(curItem, 1);
    assert(newItem != nullptr);
    return JsonValue(newItem);
}

const JsonValueRef JsonObject::operator [] (const std::string &key) const
{
    struct cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
    assert(curItem != nullptr);
    return JsonValueRef(item_, curItem);
}

bool JsonObject::operator == (const JsonObject &other) const
{
    if (this == &other) {
        return true;
    }
    return cJSON_Compare(item_, other.item_, 1);
}

JsonObject &JsonObject::operator = (const JsonObject &other)
{
    if (this == &other) {
        return *this;
    }

    cJSON_Delete(item_);
    item_ = cJSON_Duplicate(other.item_, 1);
    return *this;
}

JsonObject &JsonObject::operator = (JsonObject &&other)
{
    if (this == &other) {
        return *this;
    }
    std::swap(item_, other.item_);
    return *this;
}

JsonValueRef JsonObject::operator [] (const std::string &key)
{
    assert(!key.empty());
    struct cJSON *curItem = cJSON_GetObjectItem(item_, key.c_str());
    if (!curItem) {
        struct cJSON *newItem = cJSON_CreateNull();
        assert(newItem != nullptr);
        cJSON_AddItemToObject(item_, key.c_str(), newItem);
        return JsonValueRef(item_, newItem);
    }
    return JsonValueRef(item_, curItem);
}

//------------------[JsonObject] END---------------------

//------------------[JsonDocument] BEGIN---------------------
JsonDocument::JsonDocument()
    : item_(nullptr)
{

}

JsonDocument::JsonDocument(const JsonObject &object)
    : item_(nullptr)
{
    assert(cJSON_IsObject(object.item_));
    item_ = cJSON_Duplicate(object.item_, 1);
    assert(item_ != nullptr);
}

JsonDocument::JsonDocument(const JsonArray &array)
    : item_(nullptr)
{
    assert(cJSON_IsArray(array.item_));
    item_ = cJSON_Duplicate(array.item_, 1);
    assert(item_ != nullptr);
}

JsonDocument::~JsonDocument()
{
    if (item_) {
        cJSON_Delete(item_);
    }
}

JsonDocument::JsonDocument(const JsonDocument &other)
    : item_(nullptr)
{
    if (other.item_) {
        item_ = cJSON_Duplicate(other.item_, 1);
        assert(item_ != nullptr);
    }
}

JsonDocument::JsonDocument(JsonDocument &&other)
    : item_(nullptr)
{
    std::swap(item_, other.item_);
}

JsonDocument &JsonDocument::operator =(const JsonDocument &other)
{
    if (this == &other) {
        return *this;
    }

    if (item_) {
        cJSON_Delete(item_);
    }

    if (other.item_) {
        item_ = cJSON_Duplicate(other.item_, 1);
        assert(item_ != nullptr);
    } else {
        item_ = nullptr;
    }
    return *this;
}

JsonDocument &JsonDocument::operator = (JsonDocument &&other)
{
    swap(other);
    return *this;
}

bool JsonDocument::operator == (const JsonDocument &other) const
{
    if (this == &other || (item_ == nullptr && other.item_ == nullptr)) {
        return true;
    }
    return cJSON_Compare(item_, other.item_, 1);
}

const JsonValue JsonDocument::operator [] (const std::string &key) const
{
    return object().value(key);
}

const JsonValue JsonDocument::operator [] (int index) const
{
    assert(index >= 0);
    JsonArray arryVal(array());
    if (index >= array().size()) {
        return JsonArray(static_cast<struct cJSON*>(nullptr));
    }
    return array().at(index);
}

JsonArray JsonDocument::array() const
{
    if (cJSON_IsArray(item_)) {
        return JsonArray(cJSON_Duplicate(item_, 1));
    }
    return JsonArray();
}

JsonObject JsonDocument::object() const
{
    if (cJSON_IsObject(item_)) {
        return JsonObject(cJSON_Duplicate(item_, 1));
    }
    return JsonObject();
}

JsonDocument JsonDocument::fromJson(const std::string &data, bool *ok)
{
    struct cJSON *json = cJSON_Parse(data.c_str());
    JsonDocument document;
    assert(document.item_ == nullptr);
    document.item_ = json;
    if (ok) {
        *ok = json;
    }
    return document;
}

std::string JsonDocument::toJson(JsonFormat format) const
{
    char *json = nullptr;
    if (format == Indented) {
        json = cJSON_Print(item_);
    } else {
        json = cJSON_PrintUnformatted(item_);
    }

    if (json) {
        std::string data(json);
        cJSON_free(json); //这里需要释放内存
        return data;
    }
    return std::string();
}

void JsonDocument::setArray(const JsonArray &array)
{
    assert(array.item_ != nullptr);
    if (item_) {
        cJSON_Delete(item_);
    }
    assert(cJSON_IsArray(array.item_));
    item_ = cJSON_Duplicate(array.item_, 1);
}

void JsonDocument::setObject(const JsonObject &object)
{
    assert(object.item_ != nullptr);
    if (item_) {
        cJSON_Delete(item_);
    }
    assert(cJSON_IsObject(object.item_));
    item_ = cJSON_Duplicate(object.item_, 1);
}

std::ostream &operator << (std::ostream &os, const JsonValue &val)
{
    if (val.isObject()) {
        os << JsonDocument(val.toObject()).toJson(JsonDocument::Compact);
        return os;
    } else if (val.isArray()) {
        os << JsonDocument(val.toArray()).toJson(JsonDocument::Compact);
        return os;
    } else if (val.isBool()) {
        os << (val.toBool() ? "true" : "false");
        return os;
    } else if (val.isNull()) {
        os << "null";
        return os;
    } else if (val.isNumber()) {
        os << val.toNumber();
        return os;
    } else if (val.isString()) {
        os << val.toString();
        return os;
    } else {
        return os;
    }
}

//------------------[JsonDocument] END---------------------
