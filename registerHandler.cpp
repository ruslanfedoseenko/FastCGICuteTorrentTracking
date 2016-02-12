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
#include "Subrouter.h"
#include "NewUsersRepository.h"
#include <fastcgi2/config.h>
#include <fastcgi2/logger.h>
#include <boost/bind.hpp>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include "User.h"
#include "JsonUtils.h"
#include "FcgiHelper.h"
#include "Errors.h"
#include <boost/format.hpp>
#include <cppconn/exception.h>

RegisterHandler::RegisterHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, m_router(new Subrouter)
, m_pAuthRepo(nullptr)
{
    HandlerDescriptor* registerHandler = m_router->RegisterHandler(boost::bind(&RegisterHandler::handleRegisterRequest, this, _1, _2));
    registerHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/register/")));
    registerHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* checkMailHandler = m_router->RegisterHandler(boost::bind(&RegisterHandler::handleCheckMailRequest, this, _1, _2));
    checkMailHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/register/check-mail/")));
    checkMailHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* authHandler = m_router->RegisterHandler(boost::bind(&RegisterHandler::handleAuthRequest, this, _1, _2));
    authHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/login/")));
    authHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* authKeepAliveHandler = m_router->RegisterHandler(boost::bind(&RegisterHandler::handleAuthKeepAliveRequest, this, _1, _2));
    authKeepAliveHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/login/keep-alive/(?<auth_token>[a-fA-F0-9]{64})")));
    authKeepAliveHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
}

void RegisterHandler::onLoad()
{
    const std::string loggerComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/logger");
    m_logger = context()->findComponent<fastcgi::Logger>(loggerComponentName);
    if (!m_logger)
    {
        throw std::runtime_error("cannot get component " + loggerComponentName);
    }
    std::string mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    std::string mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    std::string mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    m_pAuthRepo.reset(new NewUsersRepository(mysql_host, mysql_user, mysql_pass));
}

void RegisterHandler::onUnload() { }

void RegisterHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    m_router->HandleRequest(request, handlerContext);
}

void RegisterHandler::handleRegisterRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext)
{
    fastcgi::DataBuffer buffer = request->requestBody();
    rapidjson::Document doc;
    if (!JsonUtils::ParseJson(doc, buffer))
    {
        FcgiHelper::WriteParseError(request, doc.GetParseError());
        return;
    }
    User user;
    if (doc.IsObject() && doc.HasMember("user"))
    {
	std::string authToken;
        try
        {
            const rapidjson::Value& userJson = doc["user"];
            user.email = JsonUtils::GetValue<std::string>(userJson, "email");
            user.password = JsonUtils::GetValue<std::string>(userJson, "password");
            user.userName = JsonUtils::GetValue<std::string>(userJson, "username");
	    authToken  = m_pAuthRepo->AddUser(user);
        }
	catch (std::logic_error& ex)
        {
            m_logger->error("Exception occurred: %s", ex.what());
            FcgiHelper::WriteError(request, RequiredDataMissing, boost::str(boost::format("Missing required data: %1%") % ex.what()));
            return;
        }
        catch (std::exception& ex)
        {
            m_logger->error("Exception occurred: %s", ex.what());
            FcgiHelper::WriteError(request, RequiredDataMissing, boost::str(boost::format("Missing required data: %1%") % ex.what()));
	    return;
        }

        
        rapidjson::Document response;
        response.SetObject();

        response.AddMember("token", rapidjson::Value(authToken.c_str(), response.GetAllocator()), response.GetAllocator());
        FcgiHelper::WriteJson(request, response);
    }
    else
    {
        FcgiHelper::WriteError(request, RequiredDataMissing, "Missing required `user` object");
    }
}

void RegisterHandler::handleAuthRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext)
{
    fastcgi::DataBuffer buffer = request->requestBody();
    rapidjson::Document doc;
    if (!JsonUtils::ParseJson(doc, buffer))
    {
        FcgiHelper::WriteParseError(request, doc.GetParseError());
        return;
    }
    if (doc.IsObject())
    {
        std::string authToken, login, password;
        try
        {
            login = doc["username"].GetString();
            password = doc["password"].GetString();
        }
        catch (std::runtime_error ex)
        {
            m_logger->error("Exception occurred: %s", ex.what());
            FcgiHelper::WriteError(request, RequiredDataMissing, boost::str(boost::format("Missing required data: %1%") % ex.what()));
            return;
        }
        catch (std::exception ex)
        {
            m_logger->error("Exception occurred: %s", ex.what());
            FcgiHelper::WriteError(request, RequiredDataMissing, boost::str(boost::format("Missing required data: %1%") % ex.what()));
            return;
        }
        authToken = m_pAuthRepo->Authentificate(login, password);
        if (authToken.empty())
        {
            FcgiHelper::WriteError(request, RequiredDataMissing, "Invalid username or password");
            return;
        }
        rapidjson::Document response;
        response.SetObject();

        response.AddMember("token", rapidjson::Value(authToken.c_str(), authToken.length()), response.GetAllocator());
        FcgiHelper::WriteJson(request, response);


    }
    else
    {
        FcgiHelper::WriteError(request, RequiredDataMissing, "Missing required root object");
    }

}

void RegisterHandler::handleAuthKeepAliveRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext) 
{
    boost::any tokenIdVal = handlerContext->getParam("auth_token");
    if (!tokenIdVal.empty())
    {
        std::string token = boost::any_cast<std::string>(tokenIdVal);
        m_logger->info("Token update recived: %s", token.c_str());
        if (!m_pAuthRepo->CheckAlive(token))
        {
            m_logger->error("Expired token update received for token %s", token.c_str());
            FcgiHelper::WriteError(request, TokenExpired, "Token expired");
            return;
        }
        rapidjson::Document response;
        response.SetObject();
        response.AddMember("state", rapidjson::Value("ok", 2), response.GetAllocator());
        FcgiHelper::WriteJson(request, response);
    }
}

void RegisterHandler::handleCheckMailRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext)
{
     fastcgi::DataBuffer buffer = request->requestBody();
    rapidjson::Document doc;
    if (!JsonUtils::ParseJson(doc, buffer))
    {
        FcgiHelper::WriteParseError(request, doc.GetParseError());
        return;
    }
    if (doc.IsObject())
    {
        std::string mail;
        try
        {
            mail = doc["mail"].GetString();
            
        }
        catch (std::runtime_error ex)
        {
            m_logger->error("Exception occurred: %s", ex.what());
            FcgiHelper::WriteError(request, RequiredDataMissing, boost::str(boost::format("Missing required data: %1%") % ex.what()));
            return;
        }
        catch (std::exception ex)
        {
            m_logger->error("Exception occurred: %s", ex.what());
            FcgiHelper::WriteError(request, RequiredDataMissing, boost::str(boost::format("Missing required data: %1%") % ex.what()));
            return;
        }
    }
    else
    {
        FcgiHelper::WriteError(request, RequiredDataMissing, "Missing required root object");
    }
}




