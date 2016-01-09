#include "ratingHandler.h"
#include <cppconn/prepared_statement.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <boost/lexical_cast.hpp>

RaitingHandler::RaitingHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)


{
    std::cout << "RaitingHandler::ctor" << std::endl;
}

void RaitingHandler::onLoad()
{
    std::cout << "RaitingHandler::onLoad" << std::endl;
    mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    writingThread_ = boost::thread(boost::bind(&RaitingHandler::QueueProcessingThread, this));
}

void RaitingHandler::onUnload()
{
    std::cout << "RaitingHandler::onUnload" << std::endl;
    stopping_ = true;
    queueCondition_.notify_one();
    writingThread_.join();
}

double RaitingHandler::GetJsonDoubleValue(const rapidjson::Value& jValue, const char* name)
{
    const rapidjson::Value& value = jValue[name];
    if (value.IsDouble())
    {
	return value.GetDouble();
    }
    else if (value.IsString())
    {
	return boost::lexical_cast<double>(value.GetString());
    }

    return -1.0;
}

void RaitingHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    if (request->getRequestMethod() == "POST")
    {

	fastcgi::DataBuffer buffer = request->requestBody();

	std::string bufferString;
	buffer.toString(bufferString);

	rapidjson::Document doc;

	doc.Parse(bufferString.c_str());

	if (doc.HasParseError())
	{
	    std::cout << "Exception parsing occured " << doc.GetParseError() << std::endl;
	    request->setStatus(500);
	    return;
	}



	rapidjson::Value::ConstMemberIterator ratingJsonObjectIter = doc.FindMember("rating");
	if (ratingJsonObjectIter != doc.MemberEnd())
	{
	    Rating rating;
	    const rapidjson::Value& ratingJsonObject = ratingJsonObjectIter->value;
	    rating.speed = GetJsonDoubleValue(ratingJsonObject, "speed");
	    rating.design = GetJsonDoubleValue(ratingJsonObject, "design");
	    rating.usability = GetJsonDoubleValue(ratingJsonObject, "usability");
	    rating.possibilities = GetJsonDoubleValue(ratingJsonObject, "possibilities");
	    rating.message = ratingJsonObject["custom_msg"].GetString();
	    rating.uid = ratingJsonObject["uid"].GetString();
	    boost::mutex::scoped_lock lock(queueMutex_);
	    queue_.push_back(rating);
	    queueCondition_.notify_one();
	}
	else
	{
	    std::cout << "Required rating json object is missing." << std::endl;
	    request->setStatus(400);
	    return;
	}

    }
    else
    {
	std::stringbuf buffer("{\"state\" : \"error\", \"errorString\" : \"Required parameter is missing.\"}");
	request->setStatus(400);
	request->write(&buffer);
    }

}

void RaitingHandler::QueueProcessingThread()
{
    sql::Driver *driver = get_driver_instance();
    driver->threadInit();
    boost::scoped_ptr<sql::Connection> con(driver->connect(mysql_host, mysql_user, mysql_pass));
    con->setSchema("tracking_db");
    while (!stopping_)
    {
	std::vector<Rating> queueCopy;
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
	    std::string query = "INSERT INTO `rating`(`uid`, `speed_mark`, `design_mark`, `possibilities_mark`, `usability_mark`, `custom_msg`) VALUES ";
	    for (int i = 0; i < queueCopy.size(); i++)
	    {
		query.append("(?, ?, ?, ?, ?, ?) ,");
	    }
	    query.erase(query.length() - 1);
	    int index = 1;
	    boost::scoped_ptr<sql::PreparedStatement> statment(con->prepareStatement(query));
	    for (std::vector<Rating>::iterator i = queueCopy.begin(); i != queueCopy.end(); ++i)
	    {
		Rating mark = *i;
		statment->setString(index++, mark.uid);
		statment->setDouble(index++, mark.speed);
		statment->setDouble(index++, mark.design);
		statment->setDouble(index++, mark.possibilities);
		statment->setDouble(index++, mark.usability);
		statment->setString(index++, mark.message);

	    }
	    statment->execute();
	}
	catch (sql::SQLException ex)
	{
	    std::cout << "sql::SQLException occured:" << ex.what() << std::endl;
	}
    }
    driver->threadEnd();
}
