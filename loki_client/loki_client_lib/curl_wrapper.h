#pragma once
/**,
 * Auth :   liubo
 * Date :   2020/10/10 16:26
 * Comment: CURL相关的  
 */

#include "curl/include/curl/curl.h"
#include <chrono>
#include <string>

enum class RequestMethod { Post, Get };
enum class ContentType { Json, Raw };

struct Response
{
	std::string body;
	int code;
};

inline size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data)
{
	if (data == nullptr)
		return 0;
	data->append(ptr, size * nmemb);
	return size * nmemb;
}

inline Response Request(CURL *curl, RequestMethod method, const std::string &url, const std::string &payload, ContentType content_type)
{
	Response r;

	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r.body);

	// 5秒超时时间
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	

	if (method == RequestMethod::Post) {
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.data());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());

		if (content_type == ContentType::Json) {
			struct curl_slist *headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		}
	}

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r.code);
	return r;
}
inline Response Get(CURL *curl, const std::string &url)
{
	return Request(curl, RequestMethod::Get, url, std::string{}, ContentType::Raw);
}

inline Response Post(CURL *curl, const std::string &url, const std::string &payload, ContentType content_type)
{
	return Request(curl, RequestMethod::Post, url, payload, content_type);
}
