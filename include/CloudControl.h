#ifndef CLOUDCONTROL_H
#define CLOUDCONTROL_H

#include <iostream>
#include "HttpUtils.h"

using namespace std;

typedef enum{GOOGLE, YANDEX} CloudType;

class CloudControl
{
public:
    CloudControl(CloudType cloud);
    void uploadFile(const string &file_path, const string &file_url);
    void downloadFile(const string& file_url, const string& file_path);
    json listDirictory(const string& uri_path);
    void getToken();

private:
    string token;
    CloudType cloud;
    HttpUtils Http;
    const string REST_YANDEX_URI = "https://cloud-api.yandex.net:443/v1/disk/resources";
};

#endif // CLOUDCONTROL_H
