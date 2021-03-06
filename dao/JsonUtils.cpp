/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   JsonUtils.cpp
 * Author: Ruslan
 * 
 * Created on February 8, 2016, 11:59 PM
 */

#include "JsonUtils.h"
#include <rapidjson/error/en.h>

bool JsonUtils::ParseJson(rapidjson::Document& doc, fastcgi::DataBuffer& buffer)
{
    std::string json;
    buffer.toString(json);



    rapidjson::ParseResult ok = doc.Parse(json.c_str());
    if (ok.IsError())
	printf( "JSON parse error: %s (%lu)\n", rapidjson::GetParseError_En(ok.Code()), ok.Offset());
    return !ok.IsError();
}

bool JsonUtils::CreateError(rapidjson::Document& document, int err, const std::string& message)
{
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.SetObject();
    document.AddMember("code", err, allocator);
    document.AddMember("message", rapidjson::Value(message.c_str(), allocator), allocator);
}

template <>
int JsonUtils::GetValue<int>(const rapidjson::Value& value, const char* path)
{
    const rapidjson::Value& subValue = value[path];
    if (subValue.IsInt())
	return subValue.GetInt();
    else
    {
	if (subValue.IsString())
	    return boost::lexical_cast<int>(subValue.GetString());
	else
	{
	    return -1;
	}
    }
}

template <>
std::string JsonUtils::GetValue<std::string>(const rapidjson::Value& value, const char* path)
{
    const rapidjson::Value& subValue = value[path];
    if (subValue.IsString())
	return subValue.GetString();
    else
    {
	return "";
    }
}
static const char* kTypeNames[] = 
    { "Null", "False", "True", "Object", "Array", "String", "Number" };
template <>
double JsonUtils::GetValue<double>(const rapidjson::Value& value, const char* path)
{
    const rapidjson::Value& subValue = value[path];
    std::cout << "ItemType" << kTypeNames[subValue.GetType()] << std::endl;
    if (subValue.IsDouble())
    {
	std::cout << "Subvalue is double" << std::endl;
	return subValue.GetDouble();
    }
    else
    {
	std::cout << "Subvalue is not double" << std::endl;
	if (subValue.IsString())
	{
	    std::cout << "Subvalue is string" << std::endl;
	    return boost::lexical_cast<double>(subValue.GetString());
	}
	else
	{
	    std::cout << "Subvalue is not a string" << std::endl;
	    return -1.0;
	}
    }
}


