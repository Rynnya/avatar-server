#ifndef controller_hpp_included
#define controller_hpp_included

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

#include "globals.hpp"
#include <filesystem>
#include <iostream>

class StaticManager
{
public:
    inline static oatpp::String readFile(std::string key, std::filesystem::path place)
    {
        if (globals::cache_state)
        {
            oatpp::String file;
            if (file = globals::cache[key].first)
            {
                globals::cache[key].second = globals::get_time_epoch();
                return file;
            }
        }

        std::string path = place.generic_string();
        auto buffer = oatpp::base::StrBuffer::loadFromFile(path.c_str());
        if (buffer)
        {
            if (globals::cache_state)
            {
                std::pair<oatpp::String, int64_t> current_pair;
                current_pair.first = buffer;
                current_pair.second = globals::get_time_epoch();
                globals::cache[key] = current_pair;
            }

            return buffer;
        }

        return nullptr;
    }
};

using namespace oatpp::web::protocol::http;
class ErrorHandler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler
{
public:
    ErrorHandler() {}

    static std::shared_ptr<ErrorHandler> createShared()
    {
        return std::make_shared<ErrorHandler>();
    }

    std::shared_ptr<outgoing::Response> handleError(const Status& status, const oatpp::String& message, const Headers& headers) override
    {
        auto response = outgoing::ResponseFactory::createResponse(Status::CODE_200, globals::cache["-1"].first);
        response->putHeader("Content-Type", "image/png");
        return response;
    };

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Controller : public oatpp::web::server::api::ApiController
{
protected:
    Controller(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {
        StaticManager::readFile("-1", std::filesystem::path(globals::path) / "-1.png");
    }
    template <typename T, typename V>
    inline static void assignOrDefault(T& value, V def) { value = value == nullptr ? def : value; }
public:
    static std::shared_ptr<Controller> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    {
        return std::shared_ptr<Controller>(new Controller(objectMapper));
    }

    ENDPOINT_ASYNC("GET", "/{id}", Root)
    {
        ENDPOINT_ASYNC_INIT(Root);

        Action act() override
        {
            String id = request->getPathVariable("id");
            assignOrDefault(id, "-1");
            std::string full_tail = id->std_str() + ".png";
            auto img = StaticManager::readFile(id->std_str(), std::filesystem::path(globals::path) / full_tail);
            assignOrDefault(img, globals::cache["-1"].first);
            auto response = controller->createResponse(Status::CODE_200, img);
            response->putHeader("Content-Type", "image/png");
            return _return(response);
        }

    };
};

#include OATPP_CODEGEN_END(ApiController) //<-- End codegen

#endif