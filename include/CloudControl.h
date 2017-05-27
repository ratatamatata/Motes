#ifndef CLOUDCONTROL_H
#define CLOUDCONTROL_H

#include <iostream>
#include <thread>
#include <memory>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <stdlib.h>
#include <curl/curl.h>
#include "json.hpp"

using namespace std;
using boost::interprocess::named_mutex;
using boost::interprocess::scoped_lock;
using boost::interprocess::shared_memory_object;
using boost::interprocess::mapped_region;
using boost::interprocess::interprocess_exception;
using boost::interprocess::offset_t;
using json = nlohmann::json;

typedef shared_ptr<shared_memory_object> shared_memory_ptr;
typedef shared_ptr<named_mutex> named_mutex_ptr;
typedef enum{GET, POST, PUT, DELETE} HTTP_Method;

class CloudControl
{
public:
    CloudControl();
    void transerToken(char *tok);
    json simpleRequest(string endOfURL, HTTP_Method method);
    bool getFirstCopy() const;

private:
    shared_memory_ptr oauth_mem;
    named_mutex_ptr mutex;
    bool firstCopy;
    string token;
};

#endif // CLOUDCONTROL_H
