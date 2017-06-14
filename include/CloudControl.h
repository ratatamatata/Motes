#ifndef CLOUDCONTROL_H
#define CLOUDCONTROL_H

#include <iostream>
#include "HttpUtils.h"

using namespace std;

typedef enum{GOOGLE, YANDEX} CloudType;

class CloudControl
{
public:
    CloudControl(CloudType cloud, const string& HOME_FOLDER = "~/.local/share/Todoom/");
    void uploadFile(const string &file_path);
    void downloadFile(const string &file_path);
    json listDirectory(const string& uri_path);
    void getToken();

private:
    string token;
    CloudType cloud;
    HttpUtils Http;
    string HOME_FOLDER;
    const string REST_YANDEX_URI = "https://cloud-api.yandex.net:443/v1/disk/resources";
    const string REST_GOOGLE_URI = "https://www.googleapis.com/drive/v2/files";
};

#endif // CLOUDCONTROL_H
