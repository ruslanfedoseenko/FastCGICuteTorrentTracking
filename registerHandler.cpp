/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   registerHandler.cpp
 * Author: Ruslan
 * 
 * Created on January 10, 2016, 11:36 AM
 */

#include "registerHandler.h"

RegisterHandler::RegisterHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, m_router(new Subrouter)

{

}

void RegisterHandler::onLoad()
{
    const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
    m_logger = context()->findComponent<fastcgi::Logger>(loggerComponentName);
    if (!m_logger) {
        throw std::runtime_error("cannot get component " + loggerComponentName);
    }
    m_logger->info("test log");
}

void RegisterHandler::onUnload()
{

}

void RegisterHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    m_router->HandleRequest(request,handlerContext);
}




