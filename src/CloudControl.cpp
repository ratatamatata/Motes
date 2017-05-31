#include "CloudControl.h"
#include <iostream>
#include <string>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/ScopedLock.h>
#include <Poco/URI.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/HTTPSClientSession.h>

using namespace std;
using namespace Poco::Net;
using namespace Poco;
using namespace Poco::Util;

bool codeReceived = false;
string code;

class CMyRequestHandler : public HTTPRequestHandler
{
public:
    void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/html");
        ostream& out = resp.send();

        URI uri(req.getURI());
        StringTokenizer str(uri.getQuery(), "=");
        if (str[0] == "code") //TODO проверку на наличие моего идентификатора
        {
            code = str[1];
            codeReceived = true;
            out << "OK\nCode is " << code;
            out.flush();
            return;
        }

        out << "error";
        out.flush();
    }
};

class MyRequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
    virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest &)
    {
        return new CMyRequestHandler;
    }
};

CloudControl::CloudControl()
{
    system("xdg-open \"https://oauth.yandex.ru/authorize?response_type=code&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b\" &");
    HTTPServer s(new MyRequestHandlerFactory, ServerSocket(8080), new HTTPServerParams);
    s.start();
    while(!codeReceived)
    {
    }
    s.stop();
    //test request
    HTTPSClientSession session("oauth.yandex.ru");
    session.setKeepAlive(true);

    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, "/token", HTTPMessage::HTTP_1_1);
    req.setContentType("application/x-www-form-urlencoded");
    req.setKeepAlive(true); // notice setKeepAlive is also called on session (above)

    std::string reqBody("grant_type=authorization_code&code=");
    reqBody += code + "&client_id=cb5b4cad0f46478c9dd05becdfd6ba6b&client_secret=68ce6e11b7ba4083895a9ba369732f54";
    req.setContentLength( reqBody.length() );

    std::ostream& myOStream = session.sendRequest(req); // sends request, returns open stream
    myOStream << reqBody;  // sends the body

    req.write(std::cout);

    Poco::Net::HTTPResponse res;
    std::istream& iStr = session.receiveResponse(res);  // get the response from server
    std::cerr << iStr.rdbuf();  // dump server response so you can view it
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

bool CloudControl::getFirstCopy() const
{
    return firstCopy;
}

