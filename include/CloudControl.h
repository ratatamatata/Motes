#ifndef CLOUDCONTROL_H
#define CLOUDCONTROL_H

#include <iostream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

typedef enum{GET, POST, PUT, DELETE} HTTP_Method;

class CloudControl
{
public:
    CloudControl();
    void transerToken(char *tok);
    json simpleRequest(string endOfURL, HTTP_Method method);
    bool getFirstCopy() const;

private:
    bool firstCopy;
    string token;
};

#endif // CLOUDCONTROL_H
