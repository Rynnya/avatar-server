#include "oatpp/network/Server.hpp"
#include "components.hpp"

#include "controller.hpp"
#include "config.hpp"

#include <iostream>

void run()
{
	/* Parse config */
	config::parse();

	/* Register Components in scope of run() method */
	AppComponent components;

	/* Get router component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	/* Add all endpoints to router */
	auto Controller = Controller::createShared();
	Controller->addEndpointsToRouter(router);

	/* Get connection handler component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	/* Get connection provider component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	/* Create server which takes provided TCP connections and passes them to HTTP connection handler */
	oatpp::network::Server server(connectionProvider, connectionHandler);

	/* Priny info about server port */
	std::cout << "Server: Running on port " << connectionProvider->getProperty("port").getData() << "\n";

	/* Initialize pseudorandom */
	srand(time(0));

	/* Run server */
	server.run();
}

int main(int argc, const char* argv[]) 
{
	oatpp::base::Environment::init();
	run();
	oatpp::base::Environment::destroy();

	return 0;
}