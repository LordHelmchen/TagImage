#include "ImaggaClient.h"
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include "multipart_parser.h"
#include <codecvt>
#include <iterator>


ImaggaClient::ImaggaClient(wstring imaggaApiKey, wstring imaggaApiSecret, wstring langCode)
{
	apiKey = imaggaApiKey;
	apiSecret = imaggaApiSecret;
	this->langCode = langCode;
}

ImaggaClient::~ImaggaClient()
{
}

bool ImaggaClient::fetch(string imageFileName)
{
	http_client_config config;
	credentials cred(apiKey, apiSecret);
	config.set_credentials(cred);
	http_client client(U("https://api.imagga.com/"), config);
	MultipartParser multi;
	multi.AddFile("image", imageFileName);
	uri_builder builder(U("uploads"));
	http_request req;
	req.set_method(http::methods::POST);
	auto boundary = multi.boundary();
	auto body = multi.GenBodyContent();
	req.set_body(body, "multipart/form-data; boundary=" + boundary);
	req.set_request_uri(uri_builder(U("v2")).append_path(U("uploads")).to_uri());
	auto requestUploadId = client.request(req)
		.then([](http_response response)
			{
				if (response.status_code() != 200)
					throw runtime_error(GetError(response));
				// Convert the response body to JSON object.
				return response.extract_json();
				/*
				{
				  "result": {
					"upload_id": "i05e132196706b94b1d85efb5f3SaM1j"
				  },
				  "status": {
					"text": "",
					"type": "success"
				  }
				}
				*/
			})
		.then([](json::value jsonObject)
			{
				return jsonObject[U("result")];
			})
				.then([](json::value jsonObject) {
				return jsonObject[U("upload_id")].as_string();
					});
	
	wstring uploadid;
	// Wait for the concurrent tasks to finish.
	try {
		requestUploadId.wait();
		uploadid = requestUploadId.get();
	}
	catch (const std::exception& e) {
		cerr << "Error exception: " << e.what();
		return false;
	}
	auto requestTags = client.request(methods::GET, uri_builder(U("v2")).append_path(U("tags")).append_query(U("image_upload_id"), uploadid).append_query(U("language"), langCode).to_string())
		.then([](http_response response)
			{
				if (response.status_code() != 200)
					throw runtime_error(GetError(response));
				// Convert the response body to JSON object.
				return response.extract_json();
			});
	try {
		requestTags.wait();
	}
	catch (const std::exception& e) {
		cerr << "Error exception: " << e.what();
		return false;
	}
	json::value jsonTags;
	// Wait for the concurrent tasks to finish.
	try {
		requestTags.wait();
		//requestTags.get() parsen und in tags() zurückgeben
		auto jsonContainer = requestTags.get();
		auto jsonResult = jsonContainer[U("result")];
		jsonTags = jsonResult[U("tags")];
	}
	catch (const std::exception& e) {
		cerr << "Error exception: " << e.what();
		return false;
	}
	if (jsonTags.is_null())
		return false;
	for each (auto t in jsonTags.as_array())
	{
		auto confidence = t[U("confidence")].as_double();
		wstring tag = t[U("tag")][U("de")].as_string();
		tagsMap[tag] = confidence;
	}

	auto requestDelete = client.request(methods::DEL, uri_builder(U("v2")).append_path(U("uploads")).append_query(U("upload_id"), uploadid).to_string())
		.then([](http_response response)
			{
				if (response.status_code() != 200)
					throw runtime_error(GetError(response));
				// Convert the response body to JSON object.
				return response.extract_json();
			});
	// Wait for the concurrent tasks to finish.
	try {
		requestDelete.wait();
	}
	catch (const std::exception& e) {
		cerr << "Error exception: " << e.what();
		return false;
	}
	return true;
}

vector<wstring> ImaggaClient::tags()
{
	vector<wstring> m_tags;
	for each (auto t in tagsMap)
		m_tags.push_back(t.first);
	return m_tags;
}

vector<string> ImaggaClient::colors()
{
	return vector<string>();
}

vector<string> ImaggaClient::tags_and_colors()
{
	return vector<string>();
}

void ImaggaClient::scrub(double confidence)
{
	for (auto it = tagsMap.begin(), ite = tagsMap.end(); it != ite;)
	{
		if (it->second < confidence)
			it = tagsMap.erase(it);
		else
			++it;
	}
}

int ImaggaClient::apiCallsLeft()
{
	return 0;
}

string ImaggaClient::GetError(http_response response)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	wstring_convert<convert_type, wchar_t> converter;
	wstring msg;
	try
	{
		msg = response.extract_json().get()[U("status")][U("error")].as_string();
	}
	catch (...) {}
	return "Returned " + std::to_string(response.status_code()) + " " + converter.to_bytes(msg);
}
