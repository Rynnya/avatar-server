#ifndef components_hpp_included
#define components_hpp_included

#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"

#include "globals.hpp"
#include "controller.hpp"
#include "cpp_redis/core/subscriber.hpp"

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent {
public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<cpp_redis::subscriber>, redis_sub)([] {

        std::shared_ptr<cpp_redis::subscriber> sub = std::make_shared<cpp_redis::subscriber>();
        sub->connect(globals::redis::address, globals::redis::port);

        if (!globals::redis::password.empty()) {
            sub->auth(globals::redis::password);
        }

        sub->subscribe("aru.change_avatar", [](const std::string& /* not used */, const std::string& message) {
            static_cast<void>(StaticManager::readFile(message, true));
        });

        sub->commit();
        return sub;

    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] {
        return std::make_shared<oatpp::async::Executor>(9, 2, 1);
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared({ "0.0.0.0", globals::port, oatpp::network::Address::IP_4 });
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {

        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor);

        auto handler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
        handler->setErrorHandler(ErrorHandler::createShared());

        return handler;

    }());

    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] {

        auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
        auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
        deserializerConfig->allowUnknownFields = false;
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);
        return objectMapper;

    }());

};

#endif /* AppComponent_hpp */