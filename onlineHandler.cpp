#include <cppconn/prepared_statement.h>
#include <boost/algorithm/string/join.hpp>
#include "onlineHandler.h"

OnlineHandler::OnlineHandler(fastcgi::ComponentContext *context)
    : fastcgi::Component(context)
{
    std::cout << "OnlineHandler::ctor" << std::endl;
}

void OnlineHandler::onLoad()
{
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

void OnlineHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    if (request->hasArg("uid"))
    {
	std::string uid = request->getArg("uid");
	int len = uid.length();

	std::cout << "OnlineHandler recived uid param: " << uid << ", Length: " << len << std::endl;
	if (len != 32)
	{
	    std::stringbuf buffer("{\"state\" : \"error\", \"errorString\" : \"UID parameter is invalid.\"}");
	    request->setStatus(400);
	    request->write(&buffer);
	}
	else
	{
	    std::cout << "Before lock handleRequest" << std::endl;
	    boost::mutex::scoped_lock lock(queueMutex_);
	    std::cout << "After lock handleRequest" << std::endl;
	    queue_.push_back(uid);
	    std::cout << "Added to quieue" << uid << ". Quieue size" << queue_.size() << std::endl;
	    ;
	    queueCondition_.notify_one();
	    std::stringbuf buffer("{\"state\" : \"ok\"}");
	    request->write(&buffer);
	}

    }
    else
    {
	std::stringbuf buffer("{\"state\" : \"error\", \"errorString\" : \"Required parameter is missing.\"}");
	request->setStatus(400);
	request->write(&buffer);
    }

}

void OnlineHandler::QueueProcessingThread()
{
    while (!stopping_)
    {
	std::vector<std::string> queueCopy;
	if (queueCopy.empty())
	{
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
	try
	{
	   m_pUserRepository->SetUsersOnline(queueCopy);
	}
	catch (sql::SQLException ex)
	{
	    std::cout << "sql::SQLException occured:" << ex.what() << ex.getSQLState() << std::endl;
	}
    }
}
