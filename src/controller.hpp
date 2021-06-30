#ifndef controller_hpp_included
#define controller_hpp_included

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "oatpp/core/base/Countable.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"

#include "config.hpp"
#include <filesystem>

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
        std::filesystem::path place = config::path;
        place /= "1.png";
        std::string path = place.generic_string();
        auto img = oatpp::base::StrBuffer::loadFromFile(path.c_str());

        return outgoing::ResponseFactory::createResponse(Status::CODE_200, img);
    };

};

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin codegen

class Controller : public oatpp::web::server::api::ApiController
{
protected:
    Controller(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {}
    template <typename T, typename V>
    static void assignOrDefault(T& value, V def) { value = value == nullptr ? def : value; }
public:
    static std::shared_ptr<Controller> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    {
        return std::shared_ptr<Controller>(new Controller(objectMapper));
    }

    ENDPOINT_ASYNC("GET", "/{id}", Root)
    {
        ENDPOINT_ASYNC_INIT(Root)

        std::shared_ptr<oatpp::base::StrBuffer> loadImage(String id)
        {
            std::filesystem::path place = config::path;
            place /= id->c_str();
            place += ".png";
            std::string path = place.generic_string();
            return oatpp::base::StrBuffer::loadFromFile(path.c_str());
        }

        Action act() override
        {
            std::shared_ptr<OutgoingResponse> response;
            String id = request->getPathVariable("id");
            assignOrDefault(id, "1");
            auto img = loadImage(id);
            assignOrDefault(img, loadImage("1"));

            response = controller->createResponse(Status::CODE_200, img);
            response->putHeader("Content-Type", "image/png");
            return _return(response);
        }

    };
};

#include OATPP_CODEGEN_END(ApiController) //<-- End codegen

#endif