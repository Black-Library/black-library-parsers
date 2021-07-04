/**
 * CurlAdapter.cc
 */

#include <CurlAdapter.h>

namespace black_library {

namespace core {

namespace parsers {

NetworkRequestResult CurlAdapter::NetworkRequest(const std::string& url) {
    NetworkRequestResult result;

    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();

    if (!curl)
    {
        result.debug_string = "Error: curl did not initialize";
        return result;
    }

    std::string html_raw;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HandleCurlResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_raw);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::stringstream ss;
        ss << "Curl Request Failed: " << curl_easy_strerror(res);
        result.debug_string = ss.str();
        return result;
    }

    curl_easy_cleanup(curl);

    result.has_error = false;
    result.html = html_raw;
    return result;
}

// Credit: https://stackoverflow.com/questions/5525613/how-do-i-fetch-a-html-page-source-with-libcurl-in-c
size_t HandleCurlResponse(void* ptr, size_t size, size_t nmemb, void* data)
{
    std::string* str = (std::string*) data;
    char* sptr = (char*) ptr;

    for (size_t x = 0; x < size * nmemb; ++x)
    {
        (*str) += sptr[x];
    }

    return size * nmemb;
}

} // namespace parsers
} // namespace core
} // namespace black_library
