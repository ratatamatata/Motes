#include "CloudControl.h"
#include <fstream>
#include <experimental/filesystem>
#include <zip.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "picosha2.h"

using namespace std;
using namespace experimental;

YandexCloudControl::YandexCloudControl(const string& HOME_FOLDER)
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
    filesystem::path p(this->HOME_FOLDER);
    if (!filesystem::exists(p)) { filesystem::create_directory(p); };
}

YandexCloudControl::~YandexCloudControl()
{
}

json YandexCloudControl::listDirectory(const string &uri_path)
{
    string url = REST_YANDEX_URI + "?path=/Приложения/Todoom/" + uri_path;
    auto responce = Http.sendRequest(url, GET, true);
    return responce;
}

void YandexCloudControl::uploadFile(const string &file_path)
{
    string url = REST_YANDEX_URI + "/upload?path=/Приложения/Todoom/" + file_path + "&overwrite=true";
    auto responce = Http.sendRequest(url, GET, true);
    string href = responce["href"];
    Http.uploadFile(HOME_FOLDER + file_path, href);
}

void YandexCloudControl::downloadFile(const string &file_path)
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
            filesystem::path path;
            if( !file_path.empty() && (file_path[file_path.size() - 1] != '/'))
            {
                path = HOME_FOLDER + file_path + '/';
            }
            else
            {
                path = HOME_FOLDER + file_path;
            }
            if (!filesystem::exists(path)) { filesystem::create_directory(path); };
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
                zip_uint64_t len = 0, size = 0;
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
            filesystem::remove(path.string() + "folder.zip");
        }
        else
        {
            auto path_end = file_path.find_last_of("/");
            if(path_end !=  string::npos)
            {
                string path_str = file_path.substr(0, path_end);
                cout << path_str << endl;
                filesystem::path path(HOME_FOLDER + path_str);
                if (!filesystem::exists(path)) { filesystem::create_directory(path); };
            }
            Http.downloadFile(HOME_FOLDER + file_path, href);
        }
}


void YandexCloudControl::deleteFile(const string &file_path)
{
        string url = REST_YANDEX_URI + "?path=/Приложения/Todoom/" + file_path + "&permanently=true";
        try
        {
            auto responce = Http.sendRequest(url, DELETE, true);
        }
        catch (nlohmann::detail::type_error& err)
        {
            clog << "File " << file_path << " was deleted" << endl;
        }
        catch (nlohmann::detail::parse_error& err)
        {
            clog << "File " << file_path << " was deleted" << endl;
        }
}

void YandexCloudControl::getToken()
{
//    system("xdg-open \"https://oauth.yandex.ru/authorize?response_type=code&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b\" &");
    code = Http.waitCode();
    string session_url;
    std::string reqBody;
    session_url = "https://oauth.yandex.ru/token";
    reqBody = "grant_type=authorization_code&code=";
    reqBody += code + "&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b&"
                          "client_secret=68ce6e11b7ba4083895a9ba369732f54";
    auto response_json = Http.sendRequest(session_url, POST, false, reqBody);
    token = response_json["access_token"];
    Http.setToken(token);
}

void YandexCloudControl::watchFolder(const string& folder)
{
    auto watch_fn = [this](const string& folder, uint32_t __mask){
        auto inotify_fd = inotify_init();
        auto wd = inotify_add_watch(inotify_fd, folder.c_str(), __mask);
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
                switch(__mask)
                {
                    case IN_MODIFY :
                    {
                        clog << "File " << changed_file << " was uploaded" << endl;
                        this->uploadFile(changed_file);
                        if(changed_file.substr(changed_file.size()-3, 3) == ".md")
                        {
                            string converted_md = folder + "/.converted/";
                            string file_name(static_cast<string>(event->name));
                            if (!filesystem::exists(converted_md)) { filesystem::create_directory(converted_md); };
                            string convert_md = "./markdown/markdown_py -x markdown.extensions.extra " +
                                                HOME_FOLDER + changed_file + " > " +
                                                converted_md + file_name.substr(0, file_name.size()-3) + ".html";
                            system(convert_md.c_str());
                            string add_css = "./markdown/add_css.py ./markdown/css.css " +
                                                converted_md + file_name.substr(0, file_name.size()-3) + ".html";
                            system(add_css.c_str());
                        }
                        break;
                    }
                    case IN_DELETE :
                    {
                        //TODO delete html
                        inotify_rm_watch( inotify_fd, wd );
                        close(inotify_fd);
                        this->deleteFile(changed_file);
                        return;
                    }
                    default: {}
                }
            }
        }
        inotify_rm_watch( inotify_fd, wd );
        close(inotify_fd);
    };
    thread(watch_fn, folder, IN_MODIFY).detach();
    thread(watch_fn, folder, IN_DELETE).detach();
    for(auto& p: filesystem::recursive_directory_iterator(folder))
    {
        if(filesystem::is_directory(p) && p.path().filename().string()[0] != '.')
        {
            thread(watch_fn, p.path().string(), IN_MODIFY).detach();
            thread(watch_fn, p.path().string(), IN_DELETE).detach();
        }
    }
}

void YandexCloudControl::syncWithCloud(const string &uri_path)
{
    auto list = this->listDirectory(uri_path)["_embedded"]["items"];
    for(auto& elem : list)
    {
        string elem_path;
        if(elem["type"] == "dir")
        {
            if(!uri_path.empty()) elem_path = uri_path + "/" +  elem["name"].get<string>();
            else elem_path = uri_path +  elem["name"].get<string>();
            this->syncWithCloud(elem_path);
            filesystem::path path(this->HOME_FOLDER +  elem_path);
            if(!filesystem::is_directory(path)) {
                filesystem::create_directory(path);
                clog << "Directory " << path.string() << " was created" << endl;
            }
        }
        else if(elem["type"] == "file")
        {
            if(!uri_path.empty()) elem_path = uri_path + "/" +  elem["name"].get<string>();
            else elem_path = uri_path +  elem["name"].get<string>();
            filesystem::path path(this->HOME_FOLDER +  elem_path);
            if(!filesystem::exists(path)) this->downloadFile(elem_path);
            else
            {
                ifstream ifs(path.string(), std::ios::binary);
                vector<unsigned char> hash(32);
                picosha2::hash256(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), hash.begin(), hash.end());
                string hex_str = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
                if(elem["sha256"] != hex_str)
                {
                    this->downloadFile(elem_path);
                    clog << "File " << path.string() << " was downloaded" << endl;
                }
            }
        }
    }
}
