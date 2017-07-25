#ifndef CLOUDCONTROL_H
#define CLOUDCONTROL_H

#include "HttpUtils.h"
#include <vector>
#include <thread>

using namespace std;

class CloudControl
{
public:
    /**
     * Upload file from cloud
     * @param file_path relative path to file on cloud
     */
    virtual void uploadFile(const string &file_path) = 0;
    /**
     * Download file or directory from cloud
     * @param file_path relative path to file or directory
     */
    virtual void downloadFile(const string &file_path) = 0;
    /**
     * Delete file or folder from cloud
     * @param file_path relative path to file or directory
     */
    virtual void deleteFile(const string &file_path) = 0;
    /**
     * Get list of files on cloud
     * @param uri_path path to cloud folder
     * @return json file which contain cloud directory structure
     */
    virtual json listDirectory(const string& uri_path) = 0;
    /**
     * Send request to cloud server to get token
     */
    virtual void getToken() = 0;
    /**
     * Create thread, that watch for modifying files in folders
     * @param folder Watched folder
     * @param os Output stream to output name of modified files
     */
    virtual void watchFolder(const string& folder) = 0;

    virtual void syncWithCloud(const string &uri_path) = 0;

protected:
    string token;
    string code;
    HttpUtils Http;
    string HOME_FOLDER;
    vector<thread> watch_thread_vec;
};

class YandexCloudControl : public CloudControl
{
public:
    /**
     * Default constructor of YandexCloudControl.
     * @param HOME_FOLDER path to application's folder
     */
    explicit YandexCloudControl(const string& HOME_FOLDER = "~/.local/share/Todoom/");
    /**
     * Destructor of YandexCloudControl. Joining all watchFolder threads
     */
    virtual ~YandexCloudControl();
    /**
     * Upload file from cloud
     * @param file_path relative path to file on cloud
     */
    void uploadFile(const string &file_path) override;
    /**
     * Download file or directory from cloud
     * @param file_path relative path to file or directory
     */
    void downloadFile(const string &file_path) override;
    /**
     * Delete file or folder from cloud
     * @param file_path relative path to file or directory
     */
    void deleteFile(const string &file_path) override;
    /**
     * Get list of files on cloud
     * @param uri_path path to cloud folder
     * @return json file which contain cloud directory structure
     */
    json listDirectory(const string& uri_path = "") override;
    /**
     * Send request to cloud server to get token
     */
    void getToken() override;
    /**
     * Create thread, that watch for modifying files in folders
     * @param folder Watched folder
     * @param os Output stream to output name of modified files
     */
    void watchFolder(const string& folder) override;

    void syncWithCloud(const string &uri_path = "") override;

private:
    const string REST_YANDEX_URI = "https://cloud-api.yandex.net:443/v1/disk/resources";
};

#endif // CLOUDCONTROL_H
