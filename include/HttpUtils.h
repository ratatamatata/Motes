#ifndef HTTPUTILS_H
#define HTTPUTILS_H

#include <iostream>
#include <string>
#include "json.hpp"
#include <curl/curl.h>
#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>

using namespace std;

using json = nlohmann::json;

typedef enum{GET, POST, PUT, DELETE} HTTP_Method;

class HttpUtils
{
public:
    /**
     * @brief HttpUtils::HttpUtils is default constructor
     */
    HttpUtils();
    ~HttpUtils();
    /**
     * @brief HttpUtils::simpleRequest
     * @param url
     * @param method
     * @param OAuth
     * @return JSON structure from server response
     */
    json sendRequest(string& url, HTTP_Method method, bool OAuth, const string &body_value = "");
    /**
     * @brief HttpUtils::waitCode
     * @return Return code from cloud server responce, for getting auth token
     */
    string waitCode();
    /**
     * @brief uploadFile
     * @param path_to_file Path to file on disk
     * @param url Path to file on server
     */
    void uploadFile(const string& path_to_file, const string& url);
    /**
     * @brief downloadFile
     * @param path_to_file Path to file on disk
     * @param url Path to file on server
     */
    void downloadFile(const string& path_to_file, const string& url);
    /**
     * @brief HttpUtils::setToken is setter for Token
     * @param Token string
     */
    void setToken(const string &value);

private:
    string token;
    CURL*  curl_handle;
    struct evhttp *server;
    struct event_base *ebase;
    string code;
};

#endif // HTTPUTILS_H
