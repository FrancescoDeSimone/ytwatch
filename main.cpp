#include <algorithm>
#include <array>
#include <curl/curl.h>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <regex>
#include <string>
#include <sys/select.h>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string download_multiplexing(std::vector<std::string> to_download) {
  std::string readBuffer;
  CURLM *multi_handle = curl_multi_init();
  curl_multi_setopt(multi_handle, CURLMOPT_MAX_HOST_CONNECTIONS, (long)1L);
  curl_multi_setopt(multi_handle, CURLMOPT_PIPELINING,
                    CURLPIPE_HTTP1 & CURLPIPE_MULTIPLEX);
  std::vector<CURL *> easy;
  for (std::string m : to_download) {
    easy.push_back(curl_easy_init());
    curl_easy_setopt(easy.back(), CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(easy.back(), CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(easy.back(), CURLOPT_URL, m.c_str());
    curl_easy_setopt(easy.back(), CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    curl_multi_add_handle(multi_handle, easy.back());
  }
  int still_running = 1;
  while (still_running) {
    curl_multi_perform(multi_handle, &still_running);
  }
  for (CURL *e : easy)
    curl_easy_cleanup(e);
  curl_multi_cleanup(multi_handle);
  return readBuffer;
}

template <typename T>
std::vector<std::vector<T>> split(std::vector<T> list, const unsigned int k) {

  std::vector<std::vector<T>> chunks;
  int part = list.size() / k;
  for (int i = 0; i < k - 2; i++) {
    std::vector<T> c = std::vector<T>{(list.begin() + (i * part)),
                                      (list.begin() + ((i + 1) * part))};
    chunks.push_back(c);
  }
  std::vector<T> c =
      std::vector<T>{list.begin() + ((k - 1) * part), list.end()};
  chunks.push_back(c);
  return chunks;
}

std::map<std::string, std::tuple<std::string, std::string, std::string>>
extract_video(std::string &s, std::string &&regex) {
  std::map<std::string, std::tuple<std::string, std::string, std::string>>
      results;
  std::smatch m;
  std::regex e(regex);
  while (std::regex_search(s, m, e)) {
    results[m[4]] = std::make_tuple(m.str(2), m.str(1), m.str(3));
    s = m.suffix().str();
  }
  return results;
}

std::map<std::string, std::tuple<std::string, std::string, std::string>>
parallel_extraction(std::vector<std::string> channels) {

  unsigned int nthreads = std::thread::hardware_concurrency();

  std::vector<std::future<std::string>> res;
  for (auto sub_channel : split<std::string>(channels, nthreads))
    res.push_back(std::async(download_multiplexing, sub_channel));

  std::string all_sub;
  for (int i = 0; i < res.size(); i++)
    all_sub += res[i].get();

  return extract_video(
      all_sub,
      "<entry>(?:.|\n)*?<title>(.*)?<(?:.|\n)*?<link rel=\"alternate\" "
      "href=\"(.*)?\"(?:.|\n)*?<name>(.*)?<(?:.|\n)*?<published>(.*)?<(?:.|\n)*"
      "?<\/entry>");

  // "<name>(.*?)<(?:.|\n)*?<updated>(.*?)<(?:.|\n)*?<media:"
  // "title>(.*?)<(?:.|\n)*?<media:content url=\"(.*?)\"");
}

std::vector<std::string> regex_matcher(std::string &s, std::string &&regex) {
  std::vector<std::string> results;
  std::smatch m;
  std::regex e(regex);
  while (std::regex_search(s, m, e)) {
    for (int i = 1; i < m.size(); i++)
      results.push_back(m[i]);
    s = m.suffix().str();
  }
  return results;
}

template <typename T>
void pprint_video(T videos,
                  std::unordered_map<std::string, std::string> options) {
  long number = std::stoi(options["number"]);
  number = number > 0 ? number : videos.size();
  for (auto [key, value] : T(videos.rbegin(), videos.rend())) {
    std::cout << key << options["delimiter"];
    std::cout << std::get<0>(value) << options["delimiter"];
    std::cout << std::get<1>(value) << options["delimiter"];
    std::cout << std::get<2>(value) << options["delimiter"];
    std::cout << "\n";
  }
}

void add_argument(std::string name, std::string sign, std::string d,
                  std::vector<std::string> argv,
                  std::unordered_map<std::string, std::string> &option) {
  auto argument = std::find(argv.begin(), argv.end(), sign);
  option[name] = argument != argv.end() ? *(argument + 1) : d;
}

std::unordered_map<std::string, std::string>
parsing_arguments(int argc, std::vector<std::string> argv) {
  std::unordered_map<std::string, std::string> options;
  add_argument("delimiter", "-d", "\t", argv, options);
  add_argument("number", "-n", "0", argv, options);

  auto file = std::find_if(argv.begin(), argv.end(), [](auto x) {
    return x.find("json") != std::string::npos;
  });
  options["file"] = file != argv.end() ? *file : "";

  if (options["file"] == "" ||
      std::find(argv.begin(), argv.end(), "-h") != argv.end()) {
    std::cout << "Usage: " << argv[0]
              << " [subscription.json]\n\t-d\tdelimiter\n\t-n\tnumber of video";
    exit(1);
  }

  return options;
}

int main(int argc, char *argv[]) {

  auto options =
      parsing_arguments(argc, std::vector<std::string>(argv + 1, argv + argc));

  std::ifstream subscription_file(options["file"]);
  std::string subscription((std::istreambuf_iterator<char>(subscription_file)),
                           std::istreambuf_iterator<char>());
  std::vector<std::string> ids =
      regex_matcher(subscription, "\"resourceId.*\n.*?\"channelId\".*?\"(.*)\"."
                                  "*?\\n.*?\"kind\" : \"youtube#channel\"");

  std::transform(ids.begin(), ids.end(), ids.begin(), [](auto x) {
    return "https://www.youtube.com/feeds/videos.xml?channel_id=" + x;
  });

  pprint_video(parallel_extraction(ids), options);

  return 0;
}
