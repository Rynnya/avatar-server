#include "oatpp/network/Server.hpp"
#include "components.hpp"

#include "controller.hpp"
#include "globals.hpp"

#include <iostream>

void run()
{
    /* Parse config */
    globals::parse();

    srand(time(0));
    AppComponent components;

    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    auto Controller = Controller::createShared();
    Controller->addEndpointsToRouter(router);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::Server server(connectionProvider, connectionHandler);

    std::cout << "Server: Running on port " << connectionProvider->getProperty("port").getData() << "\n";

    server.run();
}

int main(int argc, const char* argv[]) {

    oatpp::base::Environment::init();
    run();
    oatpp::base::Environment::destroy();

    return 0;
}