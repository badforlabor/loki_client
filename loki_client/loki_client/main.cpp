
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <chrono>

#include "../loki_client_lib/loki_client.h"


int main()
{
	auto One = InitWorker("http://10.62.117.69:3100");

	std::map<std::string, std::string> Labels;
	Labels["labelkey"] = "label_value";
	Labels["app"] = "test_cpp";

	for (int i = 0; i < 10000; i++)
	{
		One->Send(Labels, "msg:" + std::to_string(i));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	DestroyWorker(One);

	system("pause");
}