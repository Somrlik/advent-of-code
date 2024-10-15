#include "download.h"

// If without curl, implement with stumps
#ifndef USE_CURL
download_return download_puzzle_input(uint year, uint day, const std::string& key) {
    return {false, false, "", "Not compiled with CURL support."};
}
#else

#include <curl/curl.h>
#include <fmt/format.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <filesystem>

#include "fmtlog.h"
#include "trim.h"

constexpr const std::string_view AOC_URL = "https://adventofcode.com";

CURL *curl_handle = nullptr;
std::string response_buffer;

static size_t response_write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static void create_curl_handle(const std::string &session_key) {
    if (curl_handle == nullptr) {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, response_write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response_buffer);
    // curl_easy_setopt(_curl_handle, CURLOPT_FAILONERROR, 0l);
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(curl_handle, CURLOPT_COOKIELIST, "ALL");

    std::string cookie = std::string("");
    cookie.append(".adventofcode.com");
    cookie.append("\tFALSE");
    cookie.append("\t/");
    cookie.append("\tTRUE");
    cookie.append("\t0");
    cookie.append("\tsession");
    cookie.append("\t");
    cookie.append(trim(session_key));

    curl_easy_setopt(curl_handle, CURLOPT_COOKIELIST, cookie.data());
    // curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1l);
}

download_return download_puzzle_input(uint year, uint day, const std::string& key) {
    // If a handle is stale, clean it
    if (curl_handle != nullptr) {
        curl_easy_cleanup(curl_handle);

        // Sleep for a little bit to not bombard the server with subsequent requests
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    // Create a new handle
    create_curl_handle(key);

    // Build URL
    std::string url = std::string(AOC_URL);
    url.append(fmt::format("/{}/day/{}/input", year, day));
    logd("url of request is {}", url);

    // Perform the request
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(curl_handle);

    // Check for CURL errors
    if (res != CURLE_OK) {
        return {
                false,
                false,
                "",
                curl_easy_strerror(res)
        };
    }

    bool success, bad_key;
    std::string error;

    long http_code;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
    logd("response HTTP code was {}", http_code);
    if (http_code >= 500) {
        success = false;
        bad_key = false;
        error = "500 Server error";
    } else if (http_code == 404) {
        success = false;
        bad_key = false;
        error = "404 Not found";
    } else if (http_code == 400) {
        success = true;
        bad_key = true;
        error = "400 Bad request - bad key most likely";
    } else {
        success = true;
        bad_key = false;
    }

    // Return a copy of the buffer and clean it for next request
    std::string contents = response_buffer;
    response_buffer.erase();

    // Cleanup resources from curl
    curl_easy_cleanup(curl_handle);

    return {
            success,
            bad_key,
            contents,
            error
    };
}

#endif