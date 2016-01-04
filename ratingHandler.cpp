#include "ratingHandler.h"
#include <cppconn/prepared_statement.h>

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

void RaitingHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    if (request->getRequestMethod() == "POST")
    {

        fastcgi::DataBuffer buffer = request->requestBody();
        std::stringstream jsonData;
        std::string bufferString;
        buffer.toString(bufferString);
        jsonData << bufferString;
        boost::property_tree::ptree pt;
        try
        {

            boost::property_tree::read_json(jsonData, pt);

        } catch (std::exception ex)
        {
            std::cout << "Exception parsing occured " << ex.what() << std::endl;
            request->setStatus(500);
            return;
        }
        try
        {

            Rating rating;
            rating.speed = pt.get<float>("rating.speed", -1.f);
            rating.design = pt.get<float>("rating.design", -1.f);
            rating.usability = pt.get<float>("rating.usability", -1.f);
            rating.possibilities = pt.get<float>("rating.possibilities", -1.f);
            rating.message = pt.get<std::string>("rating.custom_msg");
            rating.uid = pt.get<std::string>("rating.uid");
            
            boost::mutex::scoped_lock lock(queueMutex_);
            queue_.push_back(rating);
            queueCondition_.notify_one();
        } catch (std::exception ex)
        {
            std::cout << "Exception reading data occured " << ex.what() << std::endl;
            request->setStatus(500);
            return;
        }


    } else
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
            std::cout << "After lock QueueProcessingThread" << std::endl;
        }


        boost::mutex::scoped_lock fdlock(fdMutex_);
        try
        {
            std::string query = "INSERT INTO `rating`(`uid`, `speed_mark`, `design_mark`, `possibilities_mark`, `usability_mark`, `custom_msg`) VALUES ";
            for(int i=0; i< queueCopy.size(); i++)
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
        } catch (sql::SQLException ex)
        {
            std::cout << "sql::SQLException occured:" << ex.what() << std::endl;
        }
    }
    driver->threadEnd();
}
