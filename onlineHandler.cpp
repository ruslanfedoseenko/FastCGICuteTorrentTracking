#include <cppconn/prepared_statement.h>
#include <boost/algorithm/string/join.hpp>
#include "onlineHandler.h"
#include <boost/bind.hpp>
OnlineHandler::OnlineHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, m_router(new Subrouter)
{
    HandlerDescriptor* onlineHandler = m_router->RegisterHandler(boost::bind(&OnlineHandler::handleOnlineUpdate, this, _1, _2));
    onlineHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/tracking/online")));
    onlineHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new ParamFilter("uid", "(?<uid>[a-fA-F0-9]{32})")));
    std::cout << "OnlineHandler::ctor" << std::endl;
}

void OnlineHandler::onLoad()
{
    const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
    m_logger = context()->findComponent<fastcgi::Logger>(loggerComponentName);
    if (!m_logger) {
        throw std::runtime_error("cannot get component " + loggerComponentName);
    }
    m_logger->info("test log");
    std::cout << "OnlineHandler::onLoad" << std::endl;
    mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    m_pUserRepository.reset(new UserRepository(mysql_host, mysql_user, mysql_pass));
    writingThread_ = boost::thread(boost::bind(&OnlineHandler::QueueProcessingThread, this));
}

void OnlineHandler::onUnload()
{
    std::cout << "OnlineHandler::onUnload" << std::endl;
    stopping_ = true;
    queueCondition_.notify_one();
    writingThread_.join();
}

void OnlineHandler::handleOnlineUpdate(fastcgi::Request *request,fastcgi::HandlerContext *handlerContext)
{
    std::string uid = request->getArg("uid");

    std::cout << "Before lock handleRequest" << std::endl;
    boost::mutex::scoped_lock lock(queueMutex_);
    std::cout << "After lock handleRequest" << std::endl;
    queue_.push_back(uid);
    std::cout << "Added to quieue" << uid << ". Quieue size " << queue_.size() << std::endl;
    queueCondition_.notify_one();
    std::stringbuf buffer("{\"state\" : \"ok\"}");
    request->write(&buffer);

}

void OnlineHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    m_router->HandleRequest(request,handlerContext);

}

void OnlineHandler::QueueProcessingThread()
{
    while (!stopping_) {
        std::vector<std::string> queueCopy;
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


        try {
            m_pUserRepository->SetUsersOnline(queueCopy);
            queueCopy.clear();
        }
        catch (sql::SQLException ex) {
            std::cout << "sql::SQLException occured:" << ex.what() << ex.getSQLState() << std::endl;
        }
    }
}
