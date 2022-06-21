# cjson-qt-wrapper

参考QT的JSON类接口来封装cjson，以便在非QT环境中有方便使用的JSON类。其用法与Qt的JSON类的用法基本相同。

# 例子

与Qt json相同用法的例子

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
打印输出

```
{
	"id":	1024,
	"arry_data":	[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1024]
}

```

Qt json所没有的功能，多层json的优雅创建

```
    {
        JsonObject rootObject;
        rootObject["objectInfo"]["test_1"]["info_1"] = false;
        rootObject["objectInfo"]["test_1"]["info_2"] = JsonValue();
        rootObject["objectInfo"]["test_1"]["info_3"]["info_4"] = JsonArray({1,2,3,4,5,6,7,8,9,10});
        cout << JsonDocument(rootObject).toJson() << endl;
    }
```
打印输出

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

