#pragma once

#include <chrono>
#include <iostream>

class FCostTime
{
public:
	FCostTime();

	// 返回当前耗时
	int DumpMs();

	// 返回当前耗时，并开启下一次记录
	int Next();

private:
	std::chrono::system_clock::time_point StartTime;
};

class FAutoDumpCostTime
{
public:
	FAutoDumpCostTime(const char* InTag) : Tag(InTag) {}
	~FAutoDumpCostTime()
	{
		std::cout << Tag << "耗时：" << CostTime.DumpMs() << "(ms)" << std::endl;
	}

	FCostTime CostTime;
	std::string Tag;
};

FCostTime::FCostTime()
{
	StartTime = std::chrono::system_clock::now();
}

int FCostTime::DumpMs()
{
	auto End = std::chrono::system_clock::now();
	auto Duration = End - StartTime;
	auto Ms = std::chrono::duration_cast<std::chrono::milliseconds>(Duration);
	return int(Ms.count());
}
int FCostTime::Next()
{
	auto Ms = DumpMs();
	StartTime = std::chrono::system_clock::now();
	return Ms;
}