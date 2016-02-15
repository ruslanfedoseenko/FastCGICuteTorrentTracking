/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BaseHandler.h
 * Author: Ruslan
 *
 * Created on February 10, 2016, 12:25 AM
 */

#ifndef BASEHANDLER_H
#define BASEHANDLER_H
#include <fastcgi2/component.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include "utils/Subrouter.h"

class BaseHandler : public fastcgi::Component, virtual public fastcgi::Handler {
public:
    BaseHandler(fastcgi::ComponentContext *context);
    virtual void onLoad() = 0;
    virtual void onUnload() = 0;
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext);
protected:
    boost::scoped_ptr<Subrouter> m_router;
};

#endif /* BASEHANDLER_H */

