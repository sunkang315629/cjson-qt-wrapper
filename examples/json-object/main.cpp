#include "cjson_wrapper.h"

#include <iostream>

using std::cout;
using std::endl;

int main()
{
    JsonObject rootObject;
    rootObject.insert("id", 1024);
    JsonArray arry({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    arry.append(1024);
    rootObject.insert("arry_data", arry);

    cout << JsonDocument(rootObject).toJson(JsonDocument::Compact) << endl;
    cout << JsonDocument(rootObject).toJson(JsonDocument::Indented) << endl;
    return 0;
}
