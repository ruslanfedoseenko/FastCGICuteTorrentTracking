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
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <fastcgi2/logger.h>
#include <fastcgi2/config.h>
class RegisterHandler : virtual public fastcgi::Component, virtual public fastcgi::Handler {
public:
    RegisterHandler(fastcgi::ComponentContext *context);
    virtual void onLoad();
    virtual void onUnload();
    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext) override;
private:

};

#endif /* REGISTERHANDLER_H */

