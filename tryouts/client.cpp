#include <cpprest/http_client.h>
#include <cpprest/json.h>
#pragma comment(lib, "cpprest110_1_1")
 
using namespace web;
using namespace web::http;
using namespace web::http::client;
 
#include <iostream>
using namespace std;
 
void display_field_map_json(json::value & jvalue)
{
   if (!jvalue.is_null())
   {
      for (auto const & e : jvalue)
      {
         wcout << e.first.as_string() << L" : " << e.second.as_string() << endl;
      }
   }
}
 
pplx::task<http_response> make_task_request(http_client & client, 
                                            method mtd, 
                                            json::value const & jvalue)
{
   return (mtd == methods::GET || mtd == methods::HEAD) ? 
      client.request(mtd, L"/restdemo") : 
      client.request(mtd, L"/restdemo", jvalue);
}
 
void make_request(http_client & client, method mtd, json::value const & jvalue)
{
   make_task_request(client, mtd, jvalue)
      .then([](http_response response)
      {
         if (response.status_code() == status_codes::OK)
         {
            return response.extract_json();
         }
         return pplx::task_from_result(json::value());
      })
      .then([](pplx::task<json::value> previousTask)
      {
         try
         {
            display_field_map_json(previousTask.get());
         }
         catch (http_exception const & e)
         {
            wcout << e.what() << endl;
         }
      })
      .wait();
}