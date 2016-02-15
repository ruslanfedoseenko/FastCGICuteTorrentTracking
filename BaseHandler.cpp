/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BaseHandler.cpp
 * Author: Ruslan
 * 
 * Created on February 10, 2016, 12:25 AM
 */

#include "BaseHandler.h"
#include <fastcgi2/logger.h>
#include <fastcgi2/config.h>

BaseHandler::BaseHandler(fastcgi::ComponentContext* context)
: fastcgi::Component(context)
, m_router(new Subrouter)
{

}

void BaseHandler::handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext)
{
    request->setContentType("application/json");
    m_router->HandleRequest(request);
}

