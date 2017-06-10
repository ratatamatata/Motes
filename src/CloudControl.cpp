#include "CloudControl.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string code, upload_data;
size_t read_callback(char *buffer, size_t size, size_t nitems, string* data);

CloudControl::CloudControl(CloudType cloud_type) : cloud(cloud_type) {}

json CloudControl::listDirictory(const string& uri_path)
{
    if(cloud == YANDEX)
    {
        string url = REST_YANDEX_URI + "?path=/Приложения/Todoom/" + uri_path;
        auto responce = Http.sendRequest(url, GET, true);
        return responce;
    }
}

void CloudControl::uploadFile(const string& file_path, const string& file_url)
{
    if(cloud == YANDEX)
    {
        string url = REST_YANDEX_URI + "/upload?path=/Приложения/Todoom/" + file_url + "&overwrite=true";
        auto responce = Http.sendRequest(url, GET, true);
        string href = responce["href"];
        Http.uploadFile(file_path, href);
    }
    else if(cloud == GOOGLE)
    {
        string url = "https://www.googleapis.com/upload/drive/v2?uploadType=media";
        fstream file(file_path, std::fstream::in);
        string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Http.setBody(body);
        auto responce = Http.sendRequest(url, POST, true);
        cout << responce << endl;
    }
}

void CloudControl::downloadFile(const string& file_url, const string& file_path)
{
    if(cloud == YANDEX)
    {
        string url = REST_YANDEX_URI + "/download?path=/Приложения/Todoom/" + file_url;
        auto responce = Http.sendRequest(url, GET, true);
        string href = responce["href"];
        cout << href << endl;
        Http.downloadFile("/home/sabbat/test1.txt", href);
    }
}

void CloudControl::getToken()
{
    if(cloud == YANDEX)
    {
        system("xdg-open \"https://oauth.yandex.ru/authorize?response_type=code&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b\" &");
    }
    else if(cloud == GOOGLE)
    {
        system("xdg-open \"https://accounts.google.com/o/oauth2/v2/auth?&"
               "scope=https://www.googleapis.com/auth/drive.appfolder&"
               "redirect_uri=http://localhost:8080/todoom_test/&"
               "access_type=offline&response_type=code&"
               "client_id=667025493984-td9odtft7j4srplq2q421q39c7ojg4uo.apps.googleusercontent.com\" &");
    }
    code = Http.waitCode();
    //test request
    string session_url;
    std::string reqBody;
    if(cloud == YANDEX)
    {
        session_url = "https://oauth.yandex.ru/token";
        reqBody = "grant_type=authorization_code&code=";
        reqBody += code + "&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b&"
                          "client_secret=68ce6e11b7ba4083895a9ba369732f54";
    }
    else if(cloud == GOOGLE)
    {
        session_url = "https://www.googleapis.com/oauth2/v4/token";
        reqBody = "code=";
        reqBody += code + "&redirect_uri=http://localhost:8080/todoom_test/&"
                          "client_id=667025493984-td9odtft7j4srplq2q421q39c7ojg4uo.apps.googleusercontent.com&"
                          "client_secret=kVuD5R0hloUSDDc4kdzbx96n&scope=&"
                          "grant_type=authorization_code";

    }
    Http.setBody(reqBody);
    auto response_json = Http.sendRequest(session_url, POST, false);
//    cout << response_json << endl;
    token = response_json["access_token"];
//    cout << token << endl;
    Http.setToken(token);
}
