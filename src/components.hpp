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
class AppComponent
{
public:
    OATPP_CREATE_COMPONENT(std::shared_ptr<cpp_redis::subscriber>, redis_sub)([] 
    {
        std::shared_ptr<cpp_redis::subscriber> sub = std::make_shared<cpp_redis::subscriber>();
        sub->connect(globals::redis::address, globals::redis::port);
        if (!globals::redis::password.empty())
            sub->auth(globals::redis::password);

        sub->subscribe("aru::change_avatar", [](std::string ch, std::string msg)
        {
            std::unordered_map<std::string, std::pair<oatpp::String, int64_t>>::iterator it = globals::cache.find(msg);
            if (it != globals::cache.end())
            {
                std::cout << "Server: User with ID " + msg + " updated his avatar. Removing from cache...\n";
                globals::cache.erase(it);
            }
        });
        sub->commit();

        return sub;
    }());
    /**
     * Create Async Executor
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor)([] 
    {
        return std::make_shared<oatpp::async::Executor>(9, 2, 1);
    }());

    /**
     *  Create ConnectionProvider component which listens on the port
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] 
    {
        /* non_blocking connections should be used with AsyncHttpConnectionHandler for AsyncIO */
        return oatpp::network::tcp::server::ConnectionProvider::createShared({ "0.0.0.0", 8200, oatpp::network::Address::IP_4 });
    }());

    /**
     *  Create Router component
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] 
    {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    /**
     *  Create ConnectionHandler component which uses Router component to route requests
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] 
    {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
        OATPP_COMPONENT(std::shared_ptr<oatpp::async::Executor>, executor); // get Async executor component

        auto handler = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router, executor);
        handler->setErrorHandler(ErrorHandler::createShared());

        return handler;
    }());

    /**
     *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, apiObjectMapper)([] 
    {
        auto serializerConfig = oatpp::parser::json::mapping::Serializer::Config::createShared();
        auto deserializerConfig = oatpp::parser::json::mapping::Deserializer::Config::createShared();
        deserializerConfig->allowUnknownFields = false;
        auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared(serializerConfig, deserializerConfig);
        return objectMapper;
    }());

};

#endif /* AppComponent_hpp */