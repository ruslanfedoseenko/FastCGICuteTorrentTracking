#include "ratingHandler.h"
#include <cppconn/prepared_statement.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <boost/lexical_cast.hpp>
#include "JsonUtils.h"
#include <rapidjson/error/en.h>
RaitingHandler::RaitingHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, m_router(new Subrouter)
{
    HandlerDescriptor* addHandlerDescriptor = m_router->RegisterHandler(boost::bind(&RaitingHandler::AddRating, this, _1));
    addHandlerDescriptor->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("POST")));
    addHandlerDescriptor->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/rating")));
    std::cout << "RaitingHandler::ctor" << std::endl;
}

void RaitingHandler::onLoad()
{
    std::cout << "RaitingHandler::onLoad" << std::endl;
    mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    m_pRatingRepo.reset(new RaitingRepository(mysql_host, mysql_user, mysql_pass));
    writingThread_ = boost::thread(boost::bind(&RaitingHandler::QueueProcessingThread, this));
}

void RaitingHandler::onUnload()
{
    std::cout << "RaitingHandler::onUnload" << std::endl;
    stopping_ = true;
    queueCondition_.notify_one();
    writingThread_.join();
}

void RaitingHandler::AddRating(fastcgi::Request* request)
{
    fastcgi::DataBuffer buffer = request->requestBody();
    rapidjson::Document doc;
    
    if (!JsonUtils::ParseJson(doc, buffer))
    {
	std::stringbuf buffer("JSON parse error:");
        buffer.sputc(' ');
        const char* error = rapidjson::GetParseError_En(doc.GetParseError());
        buffer.sputn(error, strlen(error));
        request->setStatus(400);
        request->write(&buffer);
        return;
    }
    
    if (doc.HasMember("rating")) {
        Rating rating;
        const rapidjson::Value& ratingJsonObject = doc["rating"];
        rating.speed = JsonUtils::GetValue<double>(ratingJsonObject, "speed");
        rating.design = JsonUtils::GetValue<double>(ratingJsonObject, "design");
        rating.usability = JsonUtils::GetValue<double>(ratingJsonObject, "usability");
        rating.possibilities = JsonUtils::GetValue<double>(ratingJsonObject, "possibilities");
        rating.message = JsonUtils::GetValue<std::string>(ratingJsonObject, "custom_msg");
        rating.uid = JsonUtils::GetValue<std::string>(ratingJsonObject, "uid");
        boost::mutex::scoped_lock lock(queueMutex_);
        queue_.push_back(rating);
        queueCondition_.notify_one();
    }
    else {
        std::cout << "Required rating json object is missing." << std::endl;
        request->setStatus(400);
        return;
    }
}

void RaitingHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    m_router->HandleRequest(request);
}

void RaitingHandler::QueueProcessingThread()
{
    while (!stopping_) {
        std::vector<Rating> queueCopy;
        if (queueCopy.empty()) {
            std::cout << "Before lock QueueProcessingThread" << std::endl;
            boost::mutex::scoped_lock lock(queueMutex_);
            std::cout << "Waiting for new items..." << std::endl;
            queueCondition_.wait(lock);
            std::swap(queueCopy, queue_);
            if (queueCopy.empty())
                continue;
            std::cout << "After lock QueueProcessingThread" << std::endl;
        }


        boost::mutex::scoped_lock fdlock(fdMutex_);
        m_pRatingRepo->AddRatings(queueCopy);

    }
}
