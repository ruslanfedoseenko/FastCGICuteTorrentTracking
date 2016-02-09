/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   userHandler.cpp
 * Author: Ruslan
 * 
 * Created on January 9, 2016, 4:54 PM
 */

#include <fastcgi2/request.h>
#include <iosfwd>
#include <boost/filesystem.hpp>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "userHandler.h"
#include <iostream>
#include <sstream> 
#include <boost/bind.hpp>
#include <string>
#include <unordered_map>
#include "utils/TimeUtils.h"
#include "JsonUtils.h"
#include <rapidjson/error/en.h>


typedef std::unordered_map<std::string, std::string> stringmap;

using namespace boost::filesystem;

UserHandler::UserHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, m_queueProcessingThread(boost::bind(&UserHandler::QueueProcessingRoutine, this))
, m_router(new Subrouter)
{
    HandlerDescriptor* getOnlineHandler = m_router->RegisterHandler(boost::bind(&UserHandler::GetOnlineCount, this, _1));
    getOnlineHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/user/online/")));
    getOnlineHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("get")));
    HandlerDescriptor* getUserNameHandler = m_router->RegisterHandler(boost::bind(&UserHandler::GetUserName, this, _1));
    getUserNameHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/user/[a-fA-F0-9]{32}/")));
    getUserNameHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("get")));
    HandlerDescriptor* updateUserSessionHandler = m_router->RegisterHandler(boost::bind(&UserHandler::UpdateUserSession, this, _1));
    updateUserSessionHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/user/online/[a-fA-F0-9]{32}/")));
    updateUserSessionHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* updateUserNameHandler = m_router->RegisterHandler(boost::bind(&UserHandler::UpdateUserName, this, _1));
    updateUserNameHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/user/[a-fA-F0-9]{32}/")));
    updateUserNameHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));

}

void UserHandler::onLoad()
{
    std::string mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    std::string mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    std::string mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    m_pUserRepository.reset(new UserRepository(mysql_host, mysql_user, mysql_pass));

}

void UserHandler::onUnload()
{
    m_isStoping = true;
    m_queueCondition.notify_all();
    m_queueProcessingThread.join();
}

void UserHandler::GetOnlineCount(fastcgi::Request* request)
{
    rapidjson::Document responseDock;
    responseDock.SetObject();
    rapidjson::Document::AllocatorType& allocator = responseDock.GetAllocator();
    responseDock.AddMember("online_count", m_pUserRepository->GetOnlineUsersCount(), allocator);
    rapidjson::StringBuffer outputBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(outputBuffer);
    responseDock.Accept(writer);
    request->write(outputBuffer.GetString(), outputBuffer.GetSize());
}

void UserHandler::GetUserName(fastcgi::Request* request)
{
    rapidjson::Document responseDock;
    std::string pathStr = request->getScriptName();
    path p(pathStr);
    path::iterator pathBegin(p.begin()), pathEnd(p.end());
    path::iterator userIt = std::find(pathBegin, pathEnd, "user");
    userIt++;
    std::string userID = (*userIt).string();
    responseDock.SetObject();
    rapidjson::Document::AllocatorType& allocator = responseDock.GetAllocator();
    responseDock.AddMember("name", rapidjson::Value(m_pUserRepository->GetUserName(userID).c_str(), allocator), allocator);
    rapidjson::StringBuffer outputBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(outputBuffer);
    responseDock.Accept(writer);
    request->write(outputBuffer.GetString(), outputBuffer.GetSize());
}

void UserHandler::UpdateUserSession(fastcgi::Request* request)
{
    UserRequest userRequest;
    std::string pathStr = request->getScriptName();
    path p(pathStr);
    path::iterator pathBegin(p.begin()), pathEnd(p.end());
    path::iterator userIt = std::find(pathBegin, pathEnd, "online");
    userIt++;
    std::string userID = (*userIt).string();
    time_t updateTime;
    time(&updateTime);
    userRequest.type = UserRequest::HeartBeat;
    userRequest.userToken = userID;
    userRequest.value = updateTime;
    boost::unique_lock<boost::mutex> lock(m_queueAccessMutex);
    m_requestQueue.push_back(userRequest);
    m_queueCondition.notify_one();
}

void UserHandler::UpdateUserName(fastcgi::Request* request)
{
    fastcgi::DataBuffer buffer = request->requestBody();
    rapidjson::Document doc;
    std::string json;
    buffer.toString(json);
    rapidjson::ParseResult ok = doc.Parse(json.c_str());
    if (!ok)
    {
        std::stringbuf buffer("JSON parse error:");
        buffer.sputc(' ');
        const char* error = rapidjson::GetParseError_En(ok.Code());
        buffer.sputn(error, strlen(error));
        request->setStatus(400);
        request->write(&buffer);
        return;
    }
    UserRequest userRequest;
    std::string pathStr = request->getScriptName();
    path p(pathStr);
    path::iterator pathBegin(p.begin()), pathEnd(p.end());
    path::iterator userIt = std::find(pathBegin, pathEnd, "user");
    userIt++;
    std::string userID = (*userIt).string();
    if (doc.HasMember("user")) {
        rapidjson::Value& userObject = doc["user"];
        userRequest.userToken = userID;
        userRequest.value = JsonUtils::GetValue<std::string>(userObject, "name");
        userRequest.type = UserRequest::UserNameUpdate;
        boost::unique_lock<boost::mutex> lock(m_queueAccessMutex);
        m_requestQueue.push_back(userRequest);
        m_queueCondition.notify_one();
    }
    else {
        std::stringbuf buffer("{ state: \"error\", errorMessage: \"Missing root user object\"}");
        request->setStatus(400);
        request->write(&buffer);
        return;
    }
}

void UserHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    request->setContentType("application/json");
    m_router->HandleRequest(request);
}

void UserHandler::QueueProcessingRoutine()
{
    std::vector<UserRequest> queueCopy;
    while (!m_isStoping) {
        if (queueCopy.empty()) {
            boost::unique_lock<boost::mutex> lock(m_conditionMutex);
            if (m_requestQueue.empty())
                m_queueCondition.wait(lock);
            boost::unique_lock<boost::mutex> queueLock(m_queueAccessMutex);
            queueCopy.swap(m_requestQueue);
            if (queueCopy.empty())
                continue;
        }
        stringmap online;
        stringmap userNameUpdates;
        for (std::vector<UserRequest>::iterator i = queueCopy.begin(); i != queueCopy.end(); ++i) {
            UserRequest& request = *i;
            switch (request.type)
            {
                case UserRequest::HeartBeat:
                    online.insert(std::make_pair(request.userToken, TimeUtils::ToSqlString(boost::get<time_t>(request.value))));
                    break;
                case UserRequest::UserNameUpdate:
                    userNameUpdates.insert(std::make_pair(request.userToken, boost::get<std::string>(request.value)));
                    break;
            }
        }

        m_pUserRepository->SetUsersOnline(online);
        m_pUserRepository->SetUserNames(userNameUpdates);
        queueCopy.clear();


    }
}
