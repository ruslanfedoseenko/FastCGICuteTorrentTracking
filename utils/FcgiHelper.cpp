/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FcgiHelper.cpp
 * Author: Ruslan
 * 
 * Created on February 11, 2016, 1:13 AM
 */

#include "FcgiHelper.h"
#include <rapidjson/document.h>
#include "JsonUtils.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>
#include "Errors.h"
#include <fastcgi2/request.h>
#include <boost/format.hpp>

void FcgiHelper::WriteParseError(fastcgi::Request* request, rapidjson::ParseErrorCode err)
{
    rapidjson::Document errDoc;
    std::string message = boost::str(boost::format("JSON parse error: %1%") % rapidjson::GetParseError_En(err));
    JsonUtils::CreateError(errDoc, InvalidJson, message);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    errDoc.Accept(writer);
    request->setStatus(400);
    request->setHeader("Content-Type", "application/json");
    request->write(buffer.GetString(), buffer.GetSize());
}

void FcgiHelper::WriteError(fastcgi::Request* request, int error_code, std::string message)
{
    rapidjson::Document errDoc;
    JsonUtils::CreateError(errDoc, error_code, message);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    errDoc.Accept(writer);
    request->setStatus(400);
    request->setHeader("Content-Type", "application/json");
    request->write(buffer.GetString(), buffer.GetSize());
}

void FcgiHelper::WriteJson(fastcgi::Request* request,const rapidjson::Document& doc)
{
    rapidjson::StringBuffer outputBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(outputBuffer);
    doc.Accept(writer);
    request->write(outputBuffer.GetString(), outputBuffer.GetSize());
}


