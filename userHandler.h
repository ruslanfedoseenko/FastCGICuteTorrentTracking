/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   userHandler.h
 * Author: Ruslan
 *
 * Created on January 9, 2016, 4:54 PM
 */

#ifndef USERHANDLER_H
#define USERHANDLER_H
#include <fastcgi2/component.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "UserRequest.h"
#include <vector>

class UserRepository;
class Subrouter;

class UserHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {
public:
    UserHandler(fastcgi::ComponentContext *context);
    virtual void onLoad();
    virtual void onUnload();
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void GetOnlineCount(fastcgi::Request* request, fastcgi::HandlerContext *handlerContext);
    void GetUserName(fastcgi::Request* request, fastcgi::HandlerContext *handlerContext);
    void UpdateUserSession(fastcgi::Request* request, fastcgi::HandlerContext *handlerContext);
    void UpdateUserName(fastcgi::Request* request, fastcgi::HandlerContext *handlerContext);
private:
    std::vector<UserRequest> m_requestQueue;
    UserRepository* m_pUserRepository;
    boost::scoped_ptr<Subrouter> m_router;
    bool m_isStoping;
    boost::mutex m_queueAccessMutex, m_conditionMutex;
    boost::thread m_queueProcessingThread;
    boost::condition m_queueCondition;
    void QueueProcessingRoutine();

};

#endif /* USERHANDLER_H */

