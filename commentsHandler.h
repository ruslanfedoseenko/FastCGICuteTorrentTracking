#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <fastcgi2/logger.h>
#include <fastcgi2/config.h>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
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

class CommentsHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {

    struct Comment {
        int parentCommentId, id;
        std::string comment, infohash, userToken, comentTime;
        float rating;
        std::vector<int> childComments;
    };
    std::vector<Comment> queue_;

    boost::condition queueCondition_;
    boost::mutex queueMutex_;
    boost::mutex fdMutex_;
    std::string mysql_host, mysql_user, mysql_pass;
    bool stopping_;
    // Writing thread.
    boost::thread writingThread_;
    static void buildJson(boost::property_tree::ptree* pt, std::vector<Comment>& comments);
public:
    CommentsHandler(fastcgi::ComponentContext *context);
    virtual void onLoad();
    virtual void onUnload();
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void QueueProcessingThread();
};
