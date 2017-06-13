#include "HttpUtils.h"
#include <fstream>

HttpUtils::HttpUtils()
{
    curl_handle = curl_easy_init();
}

HttpUtils::~HttpUtils()
{
    // закрываем дескриптор curl
    curl_easy_cleanup(curl_handle);
}

string HttpUtils::waitCode()
{
    // Create a new event handler
    ebase = event_base_new();
    // Create a http server using that handler
    server = evhttp_new (ebase);
    // Create callback function
    auto callback = [&](evhttp_request *request){
        // Create an answer buffer where the data to send back to the browser will be appened
        string url = evhttp_request_get_uri(request);
        auto url_iter = url.find("todoom_test/?code="); //18 signs
        if(url_iter == std::string::npos)
        {
            return;
        }
        else
        {
            url_iter += 18;
            code = url.substr(url_iter, url.size() - url_iter);
            struct evbuffer * buffer = evbuffer_new ();
            evbuffer_add (buffer, "Code is recieved", 16);
            // Add a HTTP header, an application/json for the content type here
            evhttp_add_header (evhttp_request_get_output_headers (request), "Content-Type", "text/plain");
            // Tell we're done and data should be sent back
            evhttp_send_reply(request, HTTP_OK, "OK", buffer);
            // Free up stuff
            evbuffer_free (buffer);
            event_base_loopbreak(ebase);  //TODO must to call it later?
        }
    };
    auto thunk=[](evhttp_request *request, void *privParams){ // note thunk is captureless
      (*static_cast<decltype(callback)*>(privParams))(request);
    };
    // Set the callback for "/todoom_test/"
    evhttp_set_cb(server, "/todoom_test/", thunk, &callback);
    // Listen locally on port 32001
    if(evhttp_bind_socket (server, "localhost", 8080) != 0)
    {
         cerr << "Could not bind to 127.0.0.1:8080" << endl;
    }
    // Start processing queries
    event_base_dispatch(ebase);
    // Free up stuff
    evhttp_free (server);
    event_base_free(ebase);
    return code;
}

void HttpUtils::setToken(const string &value)
{
    token = value;
}


void HttpUtils::setBody(const string &value)
{
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, value.c_str());
}

static size_t write_data(char *ptr, size_t size, size_t nmemb, string* data)
{
    if (data)
    {
        data->append(ptr, size*nmemb);
        return size*nmemb;
    }
    else return 0;  // будет ошибка
}

json HttpUtils::sendRequest(string& url, HTTP_Method method, bool OAuth)
{
    struct curl_slist *header = NULL;
    string content;
    if(curl_handle)
    {
        // задаем  url адрес
        if(OAuth)
        {
            string oauth_header = "Authorization: OAuth " + token;
            header = curl_slist_append(header, oauth_header.c_str());
        }
        switch(method)
        {
            case GET:
            {
            curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
            break;
            }
            case POST:
            {
            curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
            break;
            }
            case PUT:
            {
            curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
            break;
            }
            case DELETE:
            {
            curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
            }
        }
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, header);
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &content);
        // send the request
        CURLcode res = curl_easy_perform(curl_handle);
        // print a response
        json response_json;
        if (!res)
        {
            response_json = nlohmann::json::parse(content);
        }
        else
        {
            cerr << curl_easy_strerror(res) << endl;
        }
        curl_easy_reset(curl_handle);
        return  response_json;
    } // TODO exeption from here
}

void HttpUtils::uploadFile(const string &path_to_file, const string &url)
{
    FILE* upload_file = fopen(path_to_file.c_str() , "r");
    if(curl_handle)
    {
        /* we want to use our own read function */
//        curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, read_callback);
        /* enable uploading */
        curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
        /* specify target */
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        /* now specify which file to upload */
        curl_easy_setopt(curl_handle, CURLOPT_READDATA, upload_file);
        /* Set the size of the file to upload (optional).  If you give a *_LARGE
           option you MUST make sure that the type of the passed-in argument is a
           curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
           make sure that to pass in a type 'long' argument. */
        curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE, sizeof(upload_file));
        curl_easy_perform(curl_handle); //todo Check return
        curl_easy_reset(curl_handle);
    }
}

void HttpUtils::downloadFile(const string& path_to_file, const string& url)
{
    fstream file(path_to_file, fstream::out);
    string file_str;
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &file_str);
        CURLcode res = curl_easy_perform(curl_handle);
        if (!res)
        {
            file << file_str;
            file.close();
        }
        else
        {
            cerr << curl_easy_strerror(res) << endl;
        }
        /* always cleanup */
        curl_easy_reset(curl_handle);
    }
}
