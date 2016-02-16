/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   registerHandler.h
 * Author: Ruslan
 *
 * Created on January 10, 2016, 11:35 AM
 */

#ifndef REGISTERHANDLER_H
#define REGISTERHANDLER_H
#include <fastcgi2/component.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/smart_ptr.hpp>
namespace fastcgi {
    class Logger;
}
class Subrouter;
class NewUsersRepository;

class RegisterHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {
public:
    RegisterHandler(fastcgi::ComponentContext *context);
    virtual void onLoad();
    virtual void onUnload();
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext) override;
    void handleRegisterRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleAuthRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleAuthKeepAliveRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void handleCheckMailRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
private:
    void QueueMailProcessing();
    bool m_isStoping;
    boost::condition m_queueCondition;
    //std::vector<Comment> queue_;
    NewUsersRepository* m_pAuthRepo;
    fastcgi::Logger* m_logger;
    boost::scoped_ptr<Subrouter> m_router;
    boost::thread m_queueProcessingThread;
};

#endif /* REGISTERHANDLER_H */

