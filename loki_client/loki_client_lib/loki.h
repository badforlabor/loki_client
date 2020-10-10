/**,
 * Auth :   liubo
 * Date :   2020/10/10 15:59
 * Comment: Loki接口 https://grafana.com/docs/loki/latest/api/
 */

#pragma once

#include <map>
#include <string>
#include <vector>



 /*
 格式是：
 {
   "streams": [
	 {
	   "stream": {
		 "label": "value"
	   },
	   "values": [
		   [ "<unix epoch in nanoseconds>", "<log line>" ],
		   [ "<unix epoch in nanoseconds>", "<log line>" ]
	   ]
	 }
   ]
 }

 譬如：
 '{"streams": [{ "stream": { "foo": "bar2" }, "values": [ [ "1570818238000000000", "fizzbuzz" ] ] }]}'
 */
class JsonStream
{
public:
	std::map<std::string, std::string> Labels;
	std::vector<std::vector<std::string>> Values;
};

class JsonMessage
{
public:
	std::vector<JsonStream> Streams;
};