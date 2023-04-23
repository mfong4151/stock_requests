// for thhe libraries that are giving errors: chatGPT recommends going to here:
// https://curl.se/. 
// https://github.com/Tencent/rapidjson

#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <cstring>
#include <rapidjson/document.h>

using namespace rapidjson;



struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *) userp;

  mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);


  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

std::vector<int> extractIntsFromJSONArray(const char *jsonString, const char *field) {
  Document doc;
  doc.Parse(jsonString);
  std::vector<int> ints;

  const Value& results = doc["results"];
  for (SizeType i = 0; i < results.Size(); i++) {
    const Value& data_hash = results[i];
    ints.push_back(data_hash[field].GetInt());
  }

  return ints;
}

std::vector<std::string> extractStringsFromJSONArray(const char *jsonString, const char *field) {
  Document doc;
  doc.Parse(jsonString);
  std::vector<std::string> strings;

  const Value& results = doc["results"];
  for (SizeType i = 0; i < results.Size(); i++) {
    const Value& data_hash = results[i];
    strings.push_back(data_hash[field].GetString());
  }

  return strings;
}

std::vector<double> extractDoublesFromJSONArray(const char *jsonString, const char *field) {
  Document doc;
  doc.Parse(jsonString);
  std::vector<double> doubles;

  const Value& results = doc["results"];
  for (SizeType i = 0; i < results.Size(); i++) {
    const Value& data_hash = results[i];
    doubles.push_back(data_hash[field].GetDouble());
  }

  return doubles;
}

std::vector<long long> extractLongLongsFromJSONArray(const char *jsonString, const char *field) {
  Document doc;
  doc.Parse(jsonString);
  std::vector<long long> longlongs;

  const Value& results = doc["results"];
  for (SizeType i = 0; i < results.Size(); i++) {
    const Value& data_hash = results[i];
    longlongs.push_back(data_hash[field].GetInt64());
  }

  return longlongs;
}

int main() {
  std::string ticker = "AAPL";
  std::string timeframe = "day";
  std::string date_from = "2021-01-01";
  std::string date_to = "2021-04-01";
  std::string apiKey = "YOUR KEY HERE";

  // Set up the URL
  std::string url = "https://api.polygon.io/v2/aggs/ticker/" + ticker + "/range/" + timeframe + "/" + date_from + "/" + date_to + "?apiKey=" + apiKey;

  // Set up the cURL object
  CURL *curl = curl_easy_init();
  if(curl) {
    // Set the URL option
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the write callback function
    struct MemoryStruct chunk;
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    } else {
      std::vector<int> timeStamps = extractLongLongsFromJSONArray(chunk.memory, "t");
      std::vector<double> opens = extractDoublesFromJSONArray(chunk.memory, "o");
      std::vector<double> closes = extractDoublesFromJSONArray(chunk.memory, "c");
      std::vector<double> highs = extractDoublesFromJSONArray(chunk.memory, "h");
      std::vector<double> lows = extractDoublesFromJSONArray(chunk.memory, "l");
      std::vector<long long> vols = extractLongLongsFromJSONArray(chunk.memory, "v");
    }

    // Clean up
    curl_easy_cleanup(curl);
    free(chunk.memory);
  }

  return 0;
}
