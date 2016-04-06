#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <fastcgi2/logger.h>
#include <fastcgi2/config.h>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/property_tree/ptree.hpp>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <sstream>
#include <rapidjson/document.h>
#include "Comments.h"
class NewUsersRepository;
class CommentsRepository;
class Subrouter;


class CommentsHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {
    std::vector<Comment> queue_;
    CommentsRepository* m_pCommentsRepository;
    NewUsersRepository* m_pAuthRepo;
    boost::condition queueCondition_;
    boost::mutex queueMutex_;
    boost::mutex fdMutex_;
    std::string mysql_host, mysql_user, mysql_pass;
    bool stopping_;
    // Writing thread.
    boost::thread writingThread_;
    static void buildJson(rapidjson::Document* pt, std::vector<Comment>& comments);
    boost::scoped_ptr<Subrouter> m_pRouter;
public:
    CommentsHandler(fastcgi::ComponentContext *context);
    virtual void onLoad();
    virtual void onUnload();
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleGetCommentsRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleAddCommentRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleEditCommentsRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleDeleteCommentRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void QueueProcessingThread();
};
