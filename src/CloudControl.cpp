#include "CloudControl.h"

using namespace std;

CloudControl::CloudControl()
{
    //Open or create the shared memory object
    struct SharedMemRemover
    {
        SharedMemRemover(){shared_memory_object::remove("oauth_mem");}
        ~SharedMemRemover(){shared_memory_object::remove("oauth_mem");}
    } shrMemRemover;
    struct MutexRemover
    {
//        MutexRemover(){named_mutex::remove("named1_mutex");}
        ~MutexRemover(){named_mutex::remove("named1_mutex");}
    } mutexRemover;

    mutex = make_shared<named_mutex>(boost::interprocess::open_or_create, "named1_mutex");

    try
    {
        oauth_mem = make_shared<shared_memory_object>(boost::interprocess::create_only
                                                      ,"oauth_mem"
                                                      ,boost::interprocess::read_write);
        cout << "Shader memory created" << endl;
        firstCopy = true;
        system("chromium \"https://oauth.yandex.ru/authorize?response_type=token&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b\" &");
        try
        {
            // wait until second process gives token
            offset_t mem_size;
            do
            {
            oauth_mem->get_size(mem_size);
            }
            while(mem_size == 0);
            cout << mem_size << endl;
            // read token from shared memory
            scoped_lock<named_mutex> lock(*mutex);
            cout << "Mutex locked" << endl;
            mapped_region region(*oauth_mem, boost::interprocess::read_only);
            char *mem = static_cast<char*>(region.get_address());
            string shared_mem_data(mem, region.get_size());
            cout << shared_mem_data << endl;
            std::size_t begin_of_token = shared_mem_data.find("access_token=") + 13; //TODO избавиться от магических значений, после того, как узнаю API гуглдиска
            std::size_t end_of_token = shared_mem_data.find("&token_type=");
            cout << begin_of_token << " " << end_of_token << endl;
            token = shared_mem_data.substr(begin_of_token, end_of_token - begin_of_token);
            cout << token << endl;
        } catch (interprocess_exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }
    // if shared memory exists, run second programm's algorithms
    catch(interprocess_exception &ex)
    {
        std::cout << ex.what() << std::endl;
        firstCopy = false;
        try
        {
            oauth_mem = make_shared<shared_memory_object>(boost::interprocess::open_only
                                           ,"oauth_mem"
                                           ,boost::interprocess::read_write);
            cout << "Shared memory opened" << endl;
        }
        catch(interprocess_exception &ex)
        {
            std::cout << ex.what() << std::endl;
            cout << "Can't create or open shared memory" << endl;
        }
    }
}

void CloudControl::transerToken(char* tok)
{
    //Open or create the named mutex
    try
    {
        scoped_lock<named_mutex> lock(*mutex);
        cout << "Mutex locked" << endl;
        oauth_mem->truncate(strlen(tok));
        mapped_region region(*oauth_mem, boost::interprocess::read_write);
        cout <<  region.get_size() << endl;
        memcpy(region.get_address(),tok,strlen(tok));
    }
    catch(interprocess_exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
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

json CloudControl::simpleRequest(string endOfURL, HTTP_Method method)
{
    struct curl_slist *header = NULL;
    string content;
    string url = "https://cloud-api.yandex.net:443/v1/disk/resources/" + endOfURL;
    CURL*  curl_handle = curl_easy_init();
    if(curl_handle)
    {
        // задаем  url адрес
        string oauth_header = "Authorization: OAuth " + token;
        header = curl_slist_append(header, oauth_header.c_str());
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
        // закрываем дескриптор curl
        curl_easy_cleanup(curl_handle);
        return response_json;
    }
}

bool CloudControl::getFirstCopy() const
{
    return firstCopy;
}

