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

LOKICPP_DLL_API LokiClientWorker* InitWorker(const char* HttpUrl);
LOKICPP_DLL_API void DestroyWorker(LokiClientWorker* InWorker);
