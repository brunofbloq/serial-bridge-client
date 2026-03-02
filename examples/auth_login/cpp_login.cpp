#include <curl/curl.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  const size_t bytes = size * nmemb;
  static_cast<std::string*>(userp)->append(static_cast<char*>(contents), bytes);
  return bytes;
}

std::string RequireEnv(const char* name) {
  const char* value = std::getenv(name);
  if (!value || !*value) {
    throw std::runtime_error(std::string("missing required env var: ") + name);
  }
  return std::string(value);
}

std::string ExtractAccessToken(const std::string& json) {
  const std::string key = "\"access_token\":\"";
  const std::size_t start = json.find(key);
  if (start == std::string::npos) {
    return "";
  }
  const std::size_t token_start = start + key.size();
  const std::size_t token_end = json.find('"', token_start);
  if (token_end == std::string::npos) {
    return "";
  }
  return json.substr(token_start, token_end - token_start);
}

void CheckHttp(CURLcode rc, long http_code, const std::string& context, const std::string& body) {
  if (rc != CURLE_OK) {
    throw std::runtime_error(context + " transport error: " + curl_easy_strerror(rc));
  }
  if (http_code < 200 || http_code >= 300) {
    throw std::runtime_error(context + " failed with status " + std::to_string(http_code) + " body=" + body);
  }
}

}  // namespace

int main() {
  try {
    const std::string base = RequireEnv("BRIDGE_SERVER_URL");
    const std::string username = RequireEnv("BRIDGE_USERNAME");
    const std::string password = RequireEnv("BRIDGE_PASSWORD");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl) {
      throw std::runtime_error("failed to init curl");
    }

    std::string login_response;
    long login_status = 0;
    const std::string login_url = base + "/login";
    const std::string login_body =
        std::string("{\"username\":\"") + username + "\",\"password\":\"" + password + "\"}";

    struct curl_slist* login_headers = nullptr;
    login_headers = curl_slist_append(login_headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, login_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, login_headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &login_response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    CURLcode rc = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &login_status);
    CheckHttp(rc, login_status, "POST /login", login_response);

    const std::string token = ExtractAccessToken(login_response);
    if (token.empty()) {
      throw std::runtime_error("POST /login response missing access_token: " + login_response);
    }
    std::cout << "login status: " << login_status << "\n";
    std::cout << "jwt prefix: " << token.substr(0, 24) << "...\n";

    std::string bus_response;
    long bus_status = 0;
    const std::string bus_url = base + "/bus";
    const std::string auth_header = "Authorization: Bearer " + token;

    struct curl_slist* bus_headers = nullptr;
    bus_headers = curl_slist_append(bus_headers, auth_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, bus_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, bus_headers);
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, nullptr);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bus_response);

    rc = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &bus_status);
    CheckHttp(rc, bus_status, "GET /bus", bus_response);

    std::cout << "protected status: " << bus_status << "\n";
    std::cout << "protected payload: " << bus_response << "\n";

    curl_slist_free_all(login_headers);
    curl_slist_free_all(bus_headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << "ERROR: " << ex.what() << "\n";
    return 1;
  }
}
