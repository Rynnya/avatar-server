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
    static oatpp::String readFile(const std::string& id, bool overwrite = false);
};

namespace http = oatpp::web::protocol::http;

class ErrorHandler : public oatpp::base::Countable, public oatpp::web::server::handler::ErrorHandler {
public:
    ErrorHandler() {}

    static std::shared_ptr<ErrorHandler> createShared() {
        return std::make_shared<ErrorHandler>();
    }

    std::shared_ptr<http::outgoing::Response> handleError(const http::Status& status, const oatpp::String& message, const Headers& headers) override {
        auto response = http::outgoing::ResponseFactory::createResponse(http::Status::CODE_200, globals::default_image);
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
            int64_t safeId = std::stoll(unsafeId->std_str());

            auto response = controller->createResponse(Status::CODE_200,  StaticManager::readFile(std::to_string(safeId)));
            response->putHeader("Content-Type", "image/png");

            return _return(response);
        }

    };
};

#include OATPP_CODEGEN_END(ApiController) //<-- End codegen

#endif