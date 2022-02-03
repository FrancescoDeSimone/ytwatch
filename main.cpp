#include <fstream>
#include <curl/curl.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <future>
#include <map>
#include <numeric>
#include <regex>
#include <fmt/core.h>
#include <algorithm>

struct video_data{
    std::string title;
    std::string url;
    std::string channel;
    std::string published;
    std::string thumbnail;

    std::string get_json(){
        return fmt::vformat("{{\"title\":\"{}\",\"url\":\"{}\",\"channel\":\"{}\",\"published\":\"{}\",\"thumbnail\":\"{}\"}}",
            fmt::make_format_args(title,url,channel,published,thumbnail));
    }
};

using Video_t = std::map<std::string,video_data>;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}


Video_t
extract_video(std::string &data, std::string&& regex){
    Video_t results;
    std::smatch m;
    std::regex e(regex);
    while (std::regex_search(data, m, e)) {
        std::string title(m[1]);
        title.erase(std::remove(title.begin(), title.end(), '\\'), title.end());
        std::string thumbnail(m[5]);
        thumbnail.replace(8,12,"img.youtube.com");
        results[m[4]] = {title,m[2],m[3],m[4],thumbnail};
        data = m.suffix().str();
    }
    return results;
}

Video_t
download_multiplexing(std::vector<std::string> to_download) {
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
  for (CURL *e : easy) curl_easy_cleanup(e);
  curl_multi_cleanup(multi_handle);

  return extract_video(
          readBuffer,
          "<entry>(?:.|\n)*?<title>(.*)<\/(?:.|\n)*?<link.*?href=\"(.*)\".*(?:.|\n)*?<name>(.*)<\/(?:.|\n)*?<published>(.*)<\/(?:.|\n)*?<.*?thumbnail.*?\"(.*?)\"");
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

Video_t
ordered_subscription_list(std::ifstream& subscription){
    std::vector<std::string> to_fetch;
    for(std::string buffer;std::getline(subscription,buffer) && !buffer.empty();){
        std::stringstream ss(buffer);
        std::string id;
        std::getline(ss,id,',');
        to_fetch.push_back("https://www.youtube.com/feeds/videos.xml?channel_id="+id);
    }

    #ifdef DEBUG
        for(auto x:to_fetch)
            std::cerr<<x<<std::endl;
    #endif

    unsigned int nthreads = std::thread::hardware_concurrency();
    std::vector<std::future<Video_t>> res;
    for(auto chunk: split(to_fetch,nthreads))
        res.push_back(std::async(download_multiplexing,chunk));

    Video_t all_sub;
    for (int i = 0; i < res.size(); i++) {
      auto v = res[i].get();
      all_sub.insert(v.begin(), v.end());
    }

    return all_sub;
}


int main (int argc, char *argv[])
{
    std::ifstream subscription(argv[1]);

    { //remove header
        std::string _;
        std::getline(subscription,_);
    }

    std::stringstream ss;
    ss<<"{\"videos\":[";
    for(auto &[key,value]:ordered_subscription_list(subscription))
        ss<<value.get_json()<<",";
    ss.seekp(-1, std::ios_base::end);
    ss<<"]}"<<std::endl;
    std::cout<<ss.str();
    return 0;
}
