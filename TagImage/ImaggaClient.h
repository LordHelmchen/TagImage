#pragma once
#include <string>
#include <vector>
#include <map>
#include <cpprest/http_client.h>

using namespace std;
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

class ImaggaClient
{
public:
	ImaggaClient(wstring imaggaApiKey, wstring imaggaApiSecret, wstring langCode = L"de");
	~ImaggaClient();
	bool fetch(string imageFileName);
	vector<wstring> tags();
	vector<string> colors();
	vector<string> tags_and_colors();
	void scrub(double confidence);
	int apiCallsLeft();
	static string GetError(http_response response);
private:
	http_client buildClient();
	wstring apiKey, apiSecret, langCode;
	map<wstring, double> tagsMap;
};