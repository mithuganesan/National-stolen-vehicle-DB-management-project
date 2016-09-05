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
        
        string docs;                                                
        for (auto&& doc : cursor) {
             docs += bsoncxx::to_json(doc);
        }
        return docs;
}

void get_method_handler( const shared_ptr< Session > session )
{   
    string docus;
    const auto& request = session->get_request( );
    docus = findentry(db, request->get_path_parameter( "name" ), request->get_path_parameter( "value" ) );
    session->close(OK, docus, { { "Content-Length", ::to_string( docus.size( ) )  }, { "Content-Type", "application/json" } } );
}

int main( const int, const char** )
{   
    
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource/{name: .*}/{value: .*}" );
   
    resource->add_rule( make_shared< AcceptRule >( ) );
    resource->set_method_handler( "GET", get_method_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_default_header( "Connection", "close" );
    
    Service service;
    service.publish( resource );
    service.add_rule( make_shared< HostRule >( ) );
    service.start( settings );
    
    return EXIT_SUCCESS;
}