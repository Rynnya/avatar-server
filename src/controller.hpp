#ifndef controller_hpp_included
#define controller_hpp_included

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

#include <filesystem>
#include <iostream>

#include "globals.hpp"

class StaticManager {
public:
    inline static oatpp::String readFile(const std::string& key, const std::filesystem::path& place) {
        if (key == "-1") {
            return globals::default_image;
        }

        if (globals::cache_state) {
            if (auto file = globals::cache.get(key)) {
                return file;
            }
        }

        const std::string path = place.generic_string();
        if (auto buffer = oatpp::base::StrBuffer::loadFromFile(path.c_str())) {
            if (globals::cache_state) {
                globals::cache.insert(key, buffer);
            }

            return buffer;
        }

        return globals::default_image;
    }
};

using namespace oatpp::web::protocol::http;
class ErrorHandler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler {
public:
    ErrorHandler() {}

    static std::shared_ptr<ErrorHandler> createShared() {
        return std::make_shared<ErrorHandler>();
    }

    std::shared_ptr<outgoing::Response> handleError(const Status& status, const oatpp::String& message, const Headers& headers) override {
        auto response = outgoing::ResponseFactory::createResponse(Status::CODE_200, globals::cache.get("-1"));
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
    {}

public:
    static std::shared_ptr<Controller> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::shared_ptr<Controller>(new Controller(objectMapper));
    }

    ENDPOINT_ASYNC("GET", "/{id}", Root) {
        ENDPOINT_ASYNC_INIT(Root);

        Action act() override {
            String unsafeId = request->getPathVariable("id");
            std::string fullId = std::to_string(std::stoll(unsafeId->std_str())) + ".png";

            auto response = controller->createResponse(
                Status::CODE_200, 
                StaticManager::readFile(fullId, std::filesystem::path(globals::path) / fullId)
            );

            response->putHeader("Content-Type", "image/png");
            return _return(response);
        }

    };
};

#include OATPP_CODEGEN_END(ApiController) //<-- End codegen

#endif