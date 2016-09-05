#include <memory>
#include <cstdlib>
#include <restbed>
#include "host_rule.hpp"
#include "accept_rule.hpp"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <chrono>

#include <bsoncxx/types.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <fstream>
#include <streambuf>
#include<iostream>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


using namespace std;
using namespace restbed;
mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{}};
auto db = conn["crossover"];

string findentry(mongocxx::database db, string findname, string tofind)
{
        auto cursor = db["vehicles"].find(document{}  << findname << tofind
                                                        << finalize);
        document rootDoc{};
        auto results_array = rootDoc << "results" << open_array;

        for (auto&& docView : cursor) {
            results_array <<  bsoncxx::types::b_document{docView};
        }
        results_array << close_array;
        bsoncxx::document::value doc = rootDoc << finalize;
        return bsoncxx::to_json(doc);
}

void get_find_handler( const shared_ptr< Session > session )
{   
    string docus;
    const auto& request = session->get_request( );
    docus = findentry(db, request->get_path_parameter( "name" ), request->get_path_parameter( "value" ) );
    session->close(OK, docus, { { "Content-Length", ::to_string( docus.size( ) )  }, { "Content-Type", "application/json" } } );
}

void get_index_handler( const shared_ptr< Session > session )
{   
    const auto request = session->get_request( );
    const string filename = "index.html";
    
    ifstream stream( "./" + filename, ifstream::in );
    
    if ( stream.is_open( ) )
    {
        const string body = string( istreambuf_iterator< char >( stream ), istreambuf_iterator< char >( ) );
        
        const multimap< string, string > headers
        {
            { "Content-Type", "text/html" },
            { "Content-Length", ::to_string( body.length( ) ) }
        };
        
        session->close( OK, body, headers );
    }
    else
    {
        session->close( NOT_FOUND );
    }
}
void get_common_handler( const shared_ptr< Session > session )
{   
    string resource;
    const auto& request = session->get_request( );
    resource = request->get_path_parameter( "res" );
    bool isResource = request->has_path_parameter( "res" );
    
    if (!isResource)
    {
        get_index_handler(session); 
    }
    else if(isResource && resource == "find")
    {
        get_find_handler(session);
    }
    else 
    {
        string noAPIResp = "Sorry, please try a valid REST API";
        session->close(OK, noAPIResp, { { "Content-Length", ::to_string( noAPIResp.size( ) )  }, { "Content-Type", "text/plain" } } );
    }
}


int main( const int, const char** )
{   
    
    auto resource = make_shared< Resource >( );
    resource->set_path( "/{res: .*}/{name: .*}/{value: .*}" );
    resource->add_rule( make_shared< AcceptRule >( ) );
    resource->set_method_handler( "GET", get_common_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_default_header( "Connection", "close" );
    
    auto resource1 = make_shared< Resource >( );
    resource1->set_path( "/" );
    resource1->add_rule( make_shared< AcceptRule >( ) );
    resource1->set_method_handler( "GET", get_index_handler );
 

    Service service;
    service.publish( resource );
    service.publish( resource1 );
    service.add_rule( make_shared< HostRule >( ) );
    service.start( settings );
    
    return EXIT_SUCCESS;
}