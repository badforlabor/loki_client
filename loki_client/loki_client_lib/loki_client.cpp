
#define LOKICPP_DLL_EXPORT
#include "loki_client.h"
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <sstream>
#include <iostream>
#include "curl_wrapper.h"
#include "json.h"
#include "cost_time.h"


static std::string NanoTime()
{
	timespec t;
	timespec_get(&t, TIME_UTC);

	return std::to_string(t.tv_sec * 1000 * 1000 * 1000 + t.tv_nsec);
}

class LokiClientWorkerInside : public LokiClientWorker
{
public:
	LokiClientWorkerInside()
	{
	
	}
	void Work()
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		while (!bThreadStop)
		{
			auto StartTime = std::chrono::system_clock::now();

			DoJobThread();


			auto End = std::chrono::system_clock::now();
			auto Duration = End - StartTime;
			auto Ms = std::chrono::duration_cast<std::chrono::milliseconds>(Duration);
			if (Ms < MinWaitTime)
			{
				std::this_thread::sleep_for(MinWaitTime - Ms);
			}
		}
	}
	void DoJobThread()
	{
		// 收集信息，然后发出去
		{
			DataLock.lock();
			auto Old = DataWriteIndex;
			DataWriteIndex = (DataWriteIndex+1)%2;
			DataLock.unlock();

			auto& DataCenter = ThreadDataCenter[Old];
			// 每次发MaxSendSize
			int Begin = 0;

			while (Begin < DataCenter.size() && !bThreadStop)
			{
				int End = Begin;
				size_t Count = 0;

				while (End < DataCenter.size())
				{
					Count += DataCenter[End].size();
					End++;

					if (Count >= MaxSendSize)
					{
						break;
					}
				}

				// 发送[Begin, End)之间的数据
				if (Count > 0)
				{
					End = Begin + 1;

					std::stringstream ss;
					ss << "{\"streams\":[";
					for (auto i = Begin; i < End; i++)
					{
						if (i != Begin)
						{
							ss << ",";
						}
						ss << DataCenter[i];
					}
					ss << "]}";

					//std::cout << ss.str() << std::endl;
					{
						std::lock_guard<std::recursive_mutex> locker(CurlLock);
						if (CurlHandle)
						{
							// 测试数据
							//ss.str("");
							//ss.clear();
							//ss << "{\"streams\": [{ \"stream\": { \"foo\": \"bar2\" }, \"values\": [ [ \"1602326816000000000\", \"fizzbuzz\" ] ] }]}";

							auto Ret = Post(CurlHandle, Host, ss.str(), ContentType::Json);

							if (Ret.code != 200 && Ret.code != 204)
							{
								std::cout << "推送数据失败, code=" << Ret.code << ", body=" << Ret.body << std::endl;
							}
						}
					}
				}

				Begin = End;
			}

			DataCenter.clear();
		}
	}
	void Init(const char* HttpUrl)
	{
		Host = HttpUrl;
		Host += "/loki/api/v1/push";
		ThreadDataCenter[0].reserve(256);
		ThreadDataCenter[1].reserve(256);

		CurlHandle = curl_easy_init();

		std::thread t(std::bind(&LokiClientWorkerInside::Work, this));
		threadHandle = move(t);

	}
	void Destroy()
	{		
		{
			std::lock_guard<std::recursive_mutex> locker(CurlLock);
			curl_easy_cleanup(CurlHandle);
			CurlHandle = nullptr;
		}

		bThreadStop = true;

		// 等待线程结束
		threadHandle.join();
	}

	// Inherited via LokiClientWorker
	virtual void Send(const std::map<std::string, std::string>& Labels, const std::string & Msg) override
	{
		if (Msg.size() == 0)
		{
			return;
		}

		// 整理数据
		JsonStream Data;
		Data.Labels = Labels;
		std::vector<std::string> One;
		One.push_back(NanoTime());
		One.push_back(Msg);
		Data.Values.push_back(One);

		// 格式化成json
		std::stringstream ss;
		{
			cereal::JSONOutputArchive archive(ss, cereal::JSONOutputArchive::Options::NoIndent());
			//archive << b;
			cereal::serialize(archive, Data);
		}


		// 放到消费线程中
		if(ss.str().size() > 0)
		{
			//std::cout << ss.str() << std::endl;

			DataLock.lock();

			auto& DataCenter = ThreadDataCenter[DataWriteIndex];
			DataCenter.push_back(ss.str());

			DataLock.unlock();
		}
	}

private:
	std::string Host;

	// 线程
	std::thread threadHandle;
	bool bThreadStop = false;
	std::chrono::milliseconds MinWaitTime = std::chrono::milliseconds(1000);

	// CURL
	CURL *CurlHandle = nullptr;
	size_t MaxSendSize = 4 * 1024;
	std::recursive_mutex CurlLock;
	
	// 数据池
	std::vector<std::string> ThreadDataCenter[2];
	int DataWriteIndex = 0;
	std::recursive_mutex DataLock;
};

LOKICPP_DLL_API LokiClientWorker* InitWorker(const char* HttpUrl)
{
	auto Ret = new LokiClientWorkerInside();
	Ret->Init(HttpUrl);
	return Ret;
}

LOKICPP_DLL_API void DestroyWorker(LokiClientWorker* InWorker)
{
	if (InWorker != nullptr)
	{
		auto WorkerInside = static_cast<LokiClientWorkerInside*>(InWorker);
		if (WorkerInside)
		{
			FAutoDumpCostTime AutoDumpTime("销毁");
			WorkerInside->Destroy();
		}
		delete InWorker;
	}
}