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
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <sstream>
#include "dao/UserRepository.h"
#include "utils/Subrouter.h"

class OnlineHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {
    std::vector<std::string> queue_;
    boost::condition queueCondition_;
    boost::mutex queueMutex_;
    boost::mutex fdMutex_;
    std::string mysql_host, mysql_user, mysql_pass;
    bool stopping_;
    // Writing thread.
    fastcgi::Logger* m_logger;
    boost::thread writingThread_;
    UserRepository* m_pUserRepository;
    boost::scoped_ptr<Subrouter> m_router;
public:
    OnlineHandler(fastcgi::ComponentContext *context);

    virtual void onLoad();

    virtual void onUnload();
    void handleOnlineUpdate(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);

    void QueueProcessingThread();
};
