#include "commentsHandler.h"
#include <cppconn/prepared_statement.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <cppconn/datatype.h>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>
using namespace boost::filesystem;

CommentsHandler::CommentsHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)

{

    std::cout << "CommentsHandler::ctor" << std::endl;


}

void CommentsHandler::onLoad()
{
    std::cout << "CommentsHandler::onLoad" << std::endl;
    mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    writingThread_ = boost::thread(boost::bind(&CommentsHandler::QueueProcessingThread, this));
}

void CommentsHandler::onUnload()
{
    std::cout << "CommentsHandler::onUnload" << std::endl;
    stopping_ = true;
    queueCondition_.notify_one();
    writingThread_.join();
}

void CommentsHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    std::string pathStr = request->getScriptName();
    path p(pathStr);
    path::iterator pathBegin(p.begin()), pathEnd(p.end());
    path::iterator commentIt = std::find(pathBegin, pathEnd, "comment");
    if (commentIt == pathEnd)
    {
        std::cout << "No info hash find in url. aborting" << std::endl;
        request->setStatus(400);
        return;
    }
    commentIt++;
    std::string infoHash = (*commentIt).string();
    std::string requestMethod = request->getRequestMethod();
    std::cout << "CommentsHandler::handleRequest RequestMethod:" << requestMethod << " Info-Hash: " << infoHash << std::endl;

    if (requestMethod == "POST")
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

            Comment comment;
            comment.parentCommentId = pt.get<int>("comment.parentId", -1);
            comment.comment = pt.get<std::string>("comment.data");
            comment.infohash = infoHash;
            comment.userToken = pt.get<std::string>("comment.userToken");
            comment.rating = pt.get<float>("comment.rating", -1.f);
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
            std::string str(buffer);
            comment.comentTime = buffer;

            boost::mutex::scoped_lock lock(queueMutex_);
            queue_.push_back(comment);
            queueCondition_.notify_one();
        } catch (std::exception ex)
        {
            std::cout << "Exception reading data occured " << ex.what() << std::endl;
            request->setStatus(500);
            return;
        }

    } else if (requestMethod == "GET")
    {

        std::string scriptFileName = request->getScriptFilename();
        std::string remoteAdr = request->getRemoteAddr();
        std::cout << "Path info: " << pathStr << " Remote Address: " << remoteAdr << " Script FileName: " << scriptFileName << std::endl;
        boost::property_tree::ptree root;
        sql::Driver *driver = get_driver_instance();
        driver->threadInit();
        boost::scoped_ptr<sql::Connection> con(driver->connect(mysql_host, mysql_user, mysql_pass));
        con->setSchema("tracking_db");

        boost::mutex::scoped_lock fdlock(fdMutex_);
        boost::scoped_ptr<sql::PreparedStatement> statment(con->prepareStatement("SELECT tc1.`id`, tc1.`parent_comment_id`, tc1.`infohash`, tc1.`comment`, tc1.`uid`, tc1.`comment_date`, tc1.`rating`, (SELECT GROUP_CONCAT(tc2.`id` SEPARATOR ',') FROM `torrent_coments` as tc2 where tc2.`parent_comment_id` = tc1.`id`) AS ChildrenComments  FROM `torrent_coments` as tc1 WHERE `infohash`=\"E146E96131826C10D4949E03D028F53035C91F45\""));
        boost::scoped_ptr<sql::ResultSet> resultSet(statment->executeQuery());
        std::vector<Comment> comments;
        while (resultSet->next())
        {
            Comment comment;
            comment.id = resultSet->getInt("id");
            comment.parentCommentId = resultSet->getInt("parent_comment_id");
            comment.infohash = resultSet->getString("infohash");
            comment.comment = resultSet->getString("comment");
            comment.userToken = resultSet->getString("uid");
            comment.comentTime = resultSet->getString("comment_date");
            comment.rating = resultSet->getDouble("rating");
            std::string childrenComments = resultSet->getString("ChildrenComments");
            std::vector<std::string> strs;

            if (!childrenComments.empty())
            {
                boost::split(strs, childrenComments, boost::is_any_of(","));
                for (int i = 0; i < strs.size(); i++)
                {
                    comment.childComments.push_back(std::stoi(strs[i]));
                }
            }
            comments.push_back(comment);
        }
        buildJson(&root, comments);
        std::stringstream ss;
        write_json(ss, root);
        std::stringbuf buffer(ss.str());
        request->setHeader("Content-Type", "application/json");
        request->write(&buffer);

    }
}

void CommentsHandler::buildJson(boost::property_tree::ptree* pt, std::vector<Comment>& commentObjs)
{
    std::vector<Comment>::iterator i = commentObjs.begin();
    boost::property_tree::ptree comments;
    while (i != commentObjs.end())
    {
        i++;
        Comment commentObj = *i;
        boost::property_tree::ptree comment;
        comment.put("id", commentObj.id);
        comment.put("parent_comment_id", commentObj.parentCommentId);
        comment.put("infohash", commentObj.infohash);
        comment.put("data", commentObj.comment);
        comment.put("user_id", commentObj.userToken);
        comment.put("comment_date", commentObj.comentTime);
        comment.put("rating", commentObj.rating);
        commentObjs.erase(i);
        comments.push_back(std::make_pair("", comment));
    }
    std::cout << "Is comments storrage empty" << comments.empty() << std::endl;
    pt->put_child("comments", comments);
}

void CommentsHandler::QueueProcessingThread()
{
    sql::Driver *driver = get_driver_instance();
    driver->threadInit();
    boost::scoped_ptr<sql::Connection> con(driver->connect(mysql_host, mysql_user, mysql_pass));
    con->setSchema("tracking_db");
    while (!stopping_)
    {
        std::vector<Comment> queueCopy;
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
            for (std::vector<Comment>::iterator i = queueCopy.begin(); i != queueCopy.end(); ++i)
            {
                Comment comment = *i;
                boost::scoped_ptr<sql::PreparedStatement> statment(con->prepareStatement("INSERT INTO `torrent_coments` (`parent_comment_id`, `infohash`, `comment`, `uid`, `comment_date`, `rating`) VALUES (?, ?, ?, ?, ?, ?)"));
                if (comment.parentCommentId <= 0)
                {
                    statment->setNull(1, sql::DataType::INTEGER);
                } else
                {
                    statment->setInt(1, comment.parentCommentId);
                }
                statment->setString(2, comment.infohash);
                statment->setString(3, comment.comment);
                statment->setString(4, comment.userToken);
                statment->setDateTime(5, comment.comentTime);
                statment->setDouble(6, comment.rating);
                statment->execute();
            }
        } catch (sql::SQLException ex)
        {
            std::cout << "sql::SQLException occured:" << ex.what() << std::endl;
        }
    }
    driver->threadEnd();
}
