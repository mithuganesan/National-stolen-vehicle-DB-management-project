#include <cpprest/http_listener.h>
#include <cpprest/json.h>
// #pragma comment(lib, "cpprest110_1_1")
 
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
 
#include <iostream>
#include <map>
#include <set>
#include <string>
using namespace std;
 
#define TRACE(msg)            wcout << msg
#define TRACE_ACTION(a, k, v) wcout << a << L" (" << k << L", " << v << L")\n"
 
map<utility::string_t, utility::string_t> dictionary;
 
 
 void handle_get(http_request request)
{
   TRACE(L"\nhandle GET\n");
 
   json::value::field_map answer;
 
   for(auto const & p : dictionary)
   {
      answer.push_back(make_pair(json::value(p.first), json::value(p.second)));
   }
 
   request.reply(status_codes::OK, json::value::object(answer));
}
int main()
{
   http_listener listener(L"http://localhost/restdemo");
 
   listener.support(methods::GET, handle_get);
//    listener.support(methods::POST, handle_post);
//    listener.support(methods::PUT, handle_put);
//    listener.support(methods::DEL, handle_del);
 
   try
   {
      listener
         .open()
         .then([&listener](){TRACE(L"\nstarting to listen\n");})
         .wait();
 
      while (true);
   }
   catch (exception const & e)
   {
      wcout << e.what() << endl;
   }
 
   return 0;
}
 void handle_get(http_request request)
{
   TRACE(L"\nhandle GET\n");
 
   json::value::field_map answer;
 
   for(auto const & p : dictionary)
   {
      answer.push_back(make_pair(json::value(p.first), json::value(p.second)));
   }
 
   request.reply(status_codes::OK, json::value::object(answer));
}
void handle_request(http_request request, 
                    function<void(json::value &, json::value::field_map &)> action)
{
   json::value::field_map answer;
 
   request
      .extract_json()
      .then([&answer, &action](pplx::task<json::value> task) {
         try
         {
            auto & jvalue = task.get();
 
            if (!jvalue.is_null())
            {
               action(jvalue, answer);
            }
         }
         catch (http_exception const & e)
         {
            wcout << e.what() << endl;
         }
      })
      .wait();
 
   request.reply(status_codes::OK, json::value::object(answer));
}
void handle_post(http_request request)
{
   TRACE("\nhandle POST\n");
 
   handle_request(
      request, 
      [](json::value & jvalue, json::value::field_map & answer)
      {
         for (auto const & e : jvalue)
         {
            if (e.second.is_string())
            {
               auto key = e.second.as_string();
 
               auto pos = dictionary.find(key);
               if (pos == dictionary.end())
               {
                  answer.push_back(make_pair(json::value(key), json::value(L"<nil>")));
               }
               else
               {
                  answer.push_back(make_pair(json::value(pos->first), json::value(pos->second)));
               }
            }
         }
      }
   );
}
 
void handle_put(http_request request)
{
   TRACE("\nhandle PUT\n");
 
   handle_request(
      request,
      [](json::value & jvalue, json::value::field_map & answer)
      {
         for (auto const & e : jvalue)
         {
            if (e.first.is_string() && e.second.is_string())
            {
               auto key = e.first.as_string();
               auto value = e.second.as_string();
 
               if (dictionary.find(key) == dictionary.end())
               {
                  TRACE_ACTION(L"added", key, value);
                  answer.push_back(make_pair(json::value(key), json::value(L"<put>")));
               }
               else
               {
                  TRACE_ACTION(L"updated", key, value);
                  answer.push_back(make_pair(json::value(key), json::value(L"<updated>")));
               }
 
               dictionary[key] = value;
            }
         }
      }
   );
}
 
void handle_del(http_request request)
{
   TRACE("\nhandle DEL\n");
 
   handle_request(
      request,
      [](json::value & jvalue, json::value::field_map & answer)
      {
         set<utility::string_t> keys;
         for (auto const & e : jvalue)
         {
            if (e.second.is_string())
            {
               auto key = e.second.as_string();
 
               auto pos = dictionary.find(key);
               if (pos == dictionary.end())
               {
                  answer.push_back(make_pair(json::value(key), json::value(L"<failed>")));
               }
               else
               {
                  TRACE_ACTION(L"deleted", pos->first, pos->second);
                  answer.push_back(make_pair(json::value(key), json::value(L"<deleted>")));
                  keys.insert(key);
               }
            }
         }
 
         for (auto const & key : keys)
            dictionary.erase(key);
      }
   );
}
