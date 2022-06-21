#include "cjson_wrapper.h"

#include <iostream>

using std::cout;
using std::endl;

int main()
{
    {
        JsonObject rootObject;
        rootObject.insert("id", 1024);
        JsonArray arry({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        arry.append(1024);
        rootObject["arry_data"] = arry;

        cout << JsonDocument(rootObject).toJson() << endl;
    }

    {
        JsonObject rootObject;
        rootObject["objectInfo"]["test_1"]["info_1"] = false;
        rootObject["objectInfo"]["test_1"]["info_2"] = JsonValue();
        rootObject["objectInfo"]["test_1"]["info_3"]["info_4"] = JsonArray({1,2,3,4,5,6,7,8,9,10});
        cout << JsonDocument(rootObject).toJson() << endl;
    }

    return 0;
}
