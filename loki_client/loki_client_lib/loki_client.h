/**,
 * Auth :   liubo
 * Date :   2020/10/10 15:19
 * Comment: LokiClient接口  
 */

#pragma once

#include "dll.h"
#include <map>
#include <string>

class LokiClientWorker
{
public:
	virtual ~LokiClientWorker() {}
	virtual void Send(const std::map<std::string, std::string>& Labels, const std::string& Msg) = 0;
};

// MinWaitTimeMs:每隔多长时间发一次包；MaxSendByte:每个包体的上限
LOKICPP_DLL_API LokiClientWorker* InitWorker(const char* HttpUrl, int MinWaitTimeMs = 0, int MaxSendByte = 0);

// MaxWaitTimeMs 最多等待时间。
LOKICPP_DLL_API void DestroyWorker(LokiClientWorker* InWorker, int MaxWaitTimeMs = 0);
