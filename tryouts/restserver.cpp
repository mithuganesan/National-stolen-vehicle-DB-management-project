#include <cpprest/http_listener.h>
#include <tr1/memory>

using namespace web::http::experimental::listener;
using namespace web::http;
using namespace web;

class MyListener 
{
public:
    MyListener(const http::uri& url);

private:
    void handle_get(http_request request);
    // void handle_put(http_request request);
    // void handle_post(http_request request);
    // void handle_delete(http_request request);
 
    http_listener m_listener;        
};

MyListener::MyListener(const http::uri& url) : m_listener(http_listener(url)) 
   
{
    m_listener.support(methods::GET,
                       std::tr1::bind(&MyListener::handle_get,
                                      this,
                                      std::tr1::placeholders::_1));
    // m_listener.support(methods::PUT,
    //                    std::tr1::bind(&MyListener::handle_put,
    //                                   this,
    //                                   std::tr1::placeholders::_1));
    // m_listener.support(methods::POST,
    //                    std::tr1::bind(&MyListener::handle_post,
    //                                   this,
    //                                   std::tr1::placeholders::_1));
    // m_listener.support(methods::DEL,
    //                    std::tr1::bind(&MyListener::handle_delete,
    //                                   this,
    //                                   std::tr1::placeholders::_1));
}


void MyListener::handle_get(http_request message)
{
    message.reply(status_codes::OK, U("Hello, World!"));
};

