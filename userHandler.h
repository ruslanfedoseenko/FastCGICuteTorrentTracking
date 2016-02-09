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
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <fastcgi2/logger.h>
#include <fastcgi2/config.h>
#include "dao/UserRepository.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "UserRequest.h"
#include "utils/Subrouter.h"
#include <vector>

class UserHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {
public:
    UserHandler(fastcgi::ComponentContext *context);
    virtual void onLoad();
    virtual void onUnload();
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
    void GetOnlineCount(fastcgi::Request* request);
    void GetUserName(fastcgi::Request* request);
    void UpdateUserSession(fastcgi::Request* request);
    void UpdateUserName(fastcgi::Request* request);
private:
    std::vector<UserRequest> m_requestQueue;
    boost::scoped_ptr<UserRepository> m_pUserRepository;
    boost::scoped_ptr<Subrouter> m_router;
    bool m_isStoping;
    boost::mutex m_queueAccessMutex, m_conditionMutex;
    boost::thread m_queueProcessingThread;
    boost::condition m_queueCondition;
    void QueueProcessingRoutine();

};

#endif /* USERHANDLER_H */

