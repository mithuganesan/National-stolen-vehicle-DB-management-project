#include <memory>
#include <cstdlib>
#include <restbed>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <chrono>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <thread>
#include <fstream>
#include <streambuf>
#include<iostream>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

using namespace std;
using namespace restbed;
mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{}};
auto db = conn["crossover"];

/*
* Adds the result log for each query to the Mongo DB
*/ 
void createDBEntry(mongocxx::database db, bsoncxx::document::value Entry, string fieldName, string fieldValue)
{   
    string queryString = " Result for stolen vehicles with " + fieldName +":"+ fieldValue;
    bsoncxx::document::value logDoc =
    document{} <<  queryString << bsoncxx::types::b_document{Entry} <<finalize;
    auto result = db["log"].insert_one(std::move(logDoc));
}

/*
* Finds the Stolen Vehicle from the Mongo DB for a given 
* fieldName and fieldValue {key:value} pair
* and returns the string in the form of Json object array.
*/
string findVehicle(mongocxx::database db, string fieldName, string fieldValue) 
{
    auto cursor = db["vehicles"].find(document{}  << fieldName << fieldValue
                                                        << finalize);
    document rootDoc{};
    auto resultsArray = rootDoc << "results" << open_array;

    for (auto&& docView : cursor) {
        resultsArray <<  bsoncxx::types::b_document{docView};
    }

    resultsArray << close_array;
    bsoncxx::document::value finalDoc = rootDoc << finalize;
    createDBEntry(db, finalDoc, fieldName, fieldValue);
    return bsoncxx::to_json(finalDoc);
}

/*
* Invokes findVehicle method for the current session and 
* sends the result to the Web application.
* (displays the result in the Browser)
*/
void findVehicleHandler( const shared_ptr< Session > session )
{   
    string outputString;
    const auto& request = session->get_request( );

    outputString = findVehicle(db, request->get_path_parameter( "name" ), request->get_path_parameter( "value" ) );
                 
    session->close(OK, outputString, { { "Content-Length", ::to_string( outputString.size( ) )  },{ "Content-Type", "application/json" } } );
}  

/*
* Opens the Web page index.html for the current session 
*/
void pageOpenHandler( const shared_ptr< Session > session )
{   
    const auto request = session->get_request( );
    const string filename = "index.html";
    
    ifstream stream( "./" + filename, ifstream::in );
    
    if ( stream.is_open( ) )
    {
        const string htmlPage = string( istreambuf_iterator< char >( stream ), istreambuf_iterator< char >( ) );
        
        const multimap< string, string > headers
        {
            { "Content-Type", "text/html" },
            { "Content-Length", ::to_string( htmlPage.length( ) ) }
        };
        
        session->close( OK, htmlPage, headers );
    }
    else
    {
        session->close( NOT_FOUND );
    }
}

/*
* Invokes findVehicleHandler method if there is a valid find query
* and {key:value} pair as parameters. 
* Otherwise closes the session with error message 
*/
void commonHandler( const shared_ptr< Session > session )
{   
    string resource;
    const auto& request = session->get_request( );
    resource = request->get_path_parameter( "res" );
    bool isResource = request->has_path_parameter( "res" );
    
    if(isResource && resource == "find")
    {
        findVehicleHandler(session);
    }
    else 
    {
        string noAPIResp = "Sorry, please try a valid REST API";
        session->close(OK, noAPIResp, { { "Content-Length", ::to_string( noAPIResp.size( ) )  }, { "Content-Type", "text/plain" } } );   }                                                    
}

/*
* Gets the {key:value} pair from the web application client
* and invokes the corresponding Handler methods
* Multithreaded Service with 4 max threads
*/
int main( int argc, char* argv[] )
{   
    auto openResource = make_shared< Resource >( );
    openResource->set_path( "/" );
    openResource->set_method_handler( "GET", pageOpenHandler );

    auto findResource = make_shared< Resource >( );
    findResource->set_path( "/{res: .*}/{name: .*}/{value: .*}" );
    findResource->set_method_handler( "GET", commonHandler );
      
    auto settings = make_shared< Settings >( );
    settings->set_port( 8080 );
    settings->set_worker_limit( 4 );
    settings->set_default_header( "Connection", "close" );

    Service service;
    service.publish( openResource );
    service.publish( findResource );
    service.start( settings );
    
    return EXIT_SUCCESS;
}