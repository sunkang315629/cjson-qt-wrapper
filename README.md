# cjson-qt-wrapper

Refer to the JSON class interface of QT to encapsulate cjson, so that JSON classes can be easily used in non QT environments.
Its usage is basically the same as that of the JSON class of QT.

# example

Examples of the same usage as QT JSON

```
    {
        JsonObject rootObject;
        rootObject.insert("id", 1024);
        JsonArray arry({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        arry.append(1024);
        rootObject["arry_data"] = arry;
        cout << JsonDocument(rootObject).toJson() << endl;
    }
```
Printout

```
{
	"id":	1024,
	"arry_data":	[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1024]
}

```

Functions that Qt JSON does not have, elegant creation of multi-layer JSON

```
    {
        JsonObject rootObject;
        rootObject["objectInfo"]["test_1"]["info_1"] = false;
        rootObject["objectInfo"]["test_1"]["info_2"] = JsonValue();
        rootObject["objectInfo"]["test_1"]["info_3"]["info_4"] = JsonArray({1,2,3,4,5,6,7,8,9,10});
        cout << JsonDocument(rootObject).toJson() << endl;
    }
```
Printout

```
{
	"objectInfo":	{
		"test_1":	{
			"info_1":	false,
			"info_2":	null,
			"info_3":	{
				"info_4":	[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
			}
		}
	}
}
```

