#ifndef CLOUDCONTROL_H
#define CLOUDCONTROL_H

#include "HttpUtils.h"
#include <vector>
#include <thread>

using namespace std;

typedef enum{GOOGLE, YANDEX} CloudType;

class CloudControl
{
public:
    /**
     * Default constructor of CloudControl.
     * @param cloud type of cloud service(Today it's only yandex)
     * @param HOME_FOLDER path to application's folder
     */
    CloudControl(CloudType cloud, const string& HOME_FOLDER = "~/.local/share/Todoom/");
    /**
     * Destructor of CloudControl. Joining all watchFolder threads
     */
    virtual ~CloudControl();
    /**
     * Upload file from cloud
     * @param file_path relative path to file on cloud
     */
    void uploadFile(const string &file_path);
    /**
     * Download file or directory from cloud
     * @param file_path relative path to file or directory
     */
    void downloadFile(const string &file_path);
    /**
     * Get list of files on cloud
     * @param uri_path path to cloud folder
     * @return json file which contain cloud directory structure
     */
    json listDirectory(const string& uri_path = "");
    /**
     * Send request to cloud server to get token
     */
    void getToken();
    /**
     * Create thread, that watch for modifying files in folders
     * @param folder Watched folder
     * @param os Output stream to output name of modified files
     */
    void watchFolder(const string& folder);

    void syncWithCloud(const string &uri_path = "");

private:
    string token;
    string code;
    CloudType cloud;
    HttpUtils Http;
    string HOME_FOLDER;
    const string REST_YANDEX_URI = "https://cloud-api.yandex.net:443/v1/disk/resources";
    const string REST_GOOGLE_URI = "https://www.googleapis.com/drive/v2/files";
    vector<thread> watch_thread_vec;
};

#endif // CLOUDCONTROL_H
