int main()
{
   http_client client(U("http://localhost"));
 
   json::value::field_map putvalue;
   putvalue.push_back(make_pair(json::value(L"one"), json::value(L"100")));
   putvalue.push_back(make_pair(json::value(L"two"), json::value(L"200")));
 
   wcout << L"\nput values\n";
   make_request(client, methods::PUT, json::value::object(putvalue));
 
   auto getvalue = json::value::array();
   getvalue[0] = json::value(L"one");
   getvalue[1] = json::value(L"two");
   getvalue[2] = json::value(L"three");
 
   wcout << L"\nget values (POST)\n";
   make_request(client, methods::POST, getvalue);
 
   auto delvalue = json::value::array();
   delvalue[0] = json::value(L"one");
 
   wcout << L"\ndelete values\n";
   make_request(client, methods::DEL, delvalue);
 
   wcout << L"\nget values (POST)\n";
   make_request(client, methods::POST, getvalue);
 
   wcout << L"\nget values (GET)\n";
   make_request(client, methods::GET, json::value::null());
 
   return 0;
}