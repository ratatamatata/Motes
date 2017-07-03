#include "CloudControl.h"
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <zip.h>
#include <sys/inotify.h>

using namespace std;

CloudControl::CloudControl(CloudType cloud_type, const string& HOME_FOLDER) : cloud(cloud_type)
{
    if (not HOME_FOLDER.empty() and HOME_FOLDER[0] == '~') {
        assert(HOME_FOLDER.size() == 1 or HOME_FOLDER[1] == '/');
        this->HOME_FOLDER = getenv("HOME") + HOME_FOLDER.substr(1,HOME_FOLDER.size()-1);
        if(this->HOME_FOLDER[this->HOME_FOLDER.size() - 1] != '/')
        {
            this->HOME_FOLDER += '/';
        }
    }
    else
    {
        this->HOME_FOLDER = HOME_FOLDER;
    }
    boost::filesystem::path p(this->HOME_FOLDER);
    if (!boost::filesystem::exists(p)) { boost::filesystem::create_directory(p); };
}

CloudControl::~CloudControl()
{
    for(auto i = watch_thread_vec.begin(); i < watch_thread_vec.end(); i++)
    {
        i->join();
    }
}

json CloudControl::listDirectory(const string &uri_path)
{
    if(cloud == YANDEX)
    {
        string url = REST_YANDEX_URI + "?path=/Приложения/Todoom/" + uri_path;
        auto responce = Http.sendRequest(url, GET, true);
        return responce;
    }
    else if(cloud == GOOGLE)
    {
        string url = REST_GOOGLE_URI + uri_path;
        auto responce = Http.sendRequest(url, GET, true);
        cout << responce << endl;
    }
}

void CloudControl::uploadFile(const string &file_path)
{
    if(cloud == YANDEX)
    {
        string url = REST_YANDEX_URI + "/upload?path=/Приложения/Todoom/" + file_path + "&overwrite=true";
        auto responce = Http.sendRequest(url, GET, true);
        string href = responce["href"];
        Http.uploadFile(HOME_FOLDER + file_path, href);
    }
    else if(cloud == GOOGLE)
    {
        string url = "https://www.googleapis.com/upload/drive/v2/file&&uploadType=media"; //REST_GOOGLE_URI + file_url;
        fstream file(HOME_FOLDER + file_path, std::fstream::in);
        string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto responce = Http.sendRequest(url, POST, true, body);
    }
}

void CloudControl::downloadFile(const string &file_path)
{
    if(cloud == YANDEX)
    {
        string url;
        if(file_path[0] == '/')
        {
            url = REST_YANDEX_URI + "/download?path=/Приложения/Todoom/" + file_path.substr(1, file_path.size() - 1);
        }
        else
        {
            url = REST_YANDEX_URI + "/download?path=/Приложения/Todoom/" + file_path;
        }
        auto responce = Http.sendRequest(url, GET, true);
        string href = responce["href"];
        if(href.find("downloader.disk.yandex.ru/zip/") != string::npos)
        {
            // check existing of folder and correct path to folder
            boost::filesystem::path path;
            if((file_path.size() != 0) & (file_path[file_path.size() - 1] != '/'))
            {
                path = HOME_FOLDER + file_path + '/';
            }
            else
            {
                path = HOME_FOLDER + file_path;
            }
            if (!boost::filesystem::exists(path)) { boost::filesystem::create_directory(path); };
            string zip_path = path.string() + "folder.zip";
            Http.downloadFile(zip_path, href);
            // open archive
            int* err = nullptr;
            auto* zip_handler = zip_open(zip_path.c_str(), ZIP_RDONLY, err);
            auto num_entries = zip_get_num_entries(zip_handler, 0);
            struct zip_stat* file_stat = nullptr;
            auto root_folder = path.parent_path().parent_path();
            for (int i = 0; i < num_entries; ++i)
            {
                zip_stat_index(zip_handler, i, 0, file_stat);
                string name = root_folder.string() + "/" + file_stat->name;
                fstream unzip_f(name, fstream::out);
                char readed_file[100];
                string file_str = "";
                int len = 0, size = 0;
                auto file_inzip = zip_fopen_index(zip_handler, i, 0);
                while(size != file_stat->size)
                {
                    len = zip_fread(file_inzip, readed_file, 100);
                    size += len;
                    string tmp = readed_file;
                    file_str += tmp.substr(0, len);
                }
                unzip_f << file_str;
                unzip_f.close();
            }
            cout << path.string() + "folder.zip" << endl;
            boost::filesystem::remove(path.string() + "folder.zip");
        }
        else
        {
            auto path_end = file_path.find_last_of("/");
            if(path_end !=  string::npos)
            {
                string path_str = file_path.substr(0, path_end);
                cout << path_str << endl;
                boost::filesystem::path path(HOME_FOLDER + path_str);
                if (!boost::filesystem::exists(path)) { boost::filesystem::create_directory(path); };
            }
            Http.downloadFile(HOME_FOLDER + file_path, href);
        }
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
    auto response_json = Http.sendRequest(session_url, POST, false, reqBody);
    token = response_json["access_token"];
    Http.setToken(token);
}

void CloudControl::watchFolder(const string& folder)
{
    auto func = [this](const string& folder){
        auto inotify_fd = inotify_init();
        auto wd = inotify_add_watch(inotify_fd, folder.c_str(), IN_MODIFY);
        char readed_file[4096]
                __attribute__ ((aligned(__alignof__(struct inotify_event))));
        const struct inotify_event *event;
        while(true)
        {
            auto len = read(inotify_fd, &readed_file, sizeof(readed_file));
            if (len <= 0) break;
            for (char *ptr = readed_file; ptr < readed_file + len;
                 ptr += sizeof(struct inotify_event) + event->len)
            {
                event = (const struct inotify_event *) ptr;
                string changed_file = (folder.size() == HOME_FOLDER.size()) ? static_cast<string>(event->name) :
                                      folder.substr(HOME_FOLDER.size(), folder.size()-HOME_FOLDER.size())
                                      + "/" + static_cast<string>(event->name);
                this->uploadFile(changed_file);
            }
        }
        inotify_rm_watch( inotify_fd, wd );
        close(inotify_fd);
    };
    watch_thread_vec.push_back(thread(func, folder));
    for(auto& p: boost::filesystem::recursive_directory_iterator(folder))
    {
        if(boost::filesystem::is_directory(p)) watch_thread_vec.push_back(thread(func, p.path().string()));
    }
}

void CloudControl::syncWithCloud(const string &uri_path)
{
    auto list = this->listDirectory(uri_path)["_embedded"]["items"];
    for(auto& elem : list)
    {
        string elem_path;
        if(elem["type"] == "dir")
        {
            if(uri_path.size() != 0) elem_path = uri_path + "/" +  elem["name"].get<string>();
            else elem_path = uri_path +  elem["name"].get<string>();
            cout << elem_path << endl;
            this->syncWithCloud(elem_path);
            boost::filesystem::path path(this->HOME_FOLDER +  elem_path);
            if(!is_directory(path)) boost::filesystem::create_directory(path);
        }
        else if(elem["type"] == "file")
        {
            cout << "Name: " + elem["name"].get<string>() << endl;
            if(uri_path.size() != 0) elem_path = uri_path + "/" +  elem["name"].get<string>();
            else elem_path = uri_path +  elem["name"].get<string>();
            cout << elem_path << endl;
            this->downloadFile(elem_path);
        }
    }
}
