#include "detail.hpp"

#include <format>

#include <curl/curl.h>
#include <curl/easy.h>

std::size_t WriteCallback(void* contents, std::size_t size, std::size_t nmemb, void* userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::string seirin::detail::fetchApiData(const std::string& url, const std::string& token)
{
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    if (!token.empty())
    {
      struct curl_slist* headers = NULL;
      headers = curl_slist_append(headers, std::format("Authorization: Bearer {}", token).c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  return readBuffer;
}