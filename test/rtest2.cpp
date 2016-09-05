#include <memory>
#include <cstdlib>
#include <restbed>
#include "host_rule.hpp"
#include "accept_rule.hpp"

using namespace std;
using namespace restbed;

void get_method_handler( const shared_ptr< Session > session )
{
    session->close( OK, "Hello, World!", { { "Content-Length", "13" }, { "Content-Type", "text/html" } } );
}

int main( const int, const char** )
{
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
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