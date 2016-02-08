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
#include <exception>
rapidjson::Document JsonUtils::ParseJson(fastcgi::DataBuffer& buffer)
{
    std::string json;
    buffer.toString(json);

    rapidjson::Document doc;

    rapidjson::ParseResult ok = doc.Parse(json.c_str());
    if (!ok)
	printf( "JSON parse error: %s (%lu)\n", rapidjson::GetParseError_En(ok.Code()), ok.Offset());
}


template <>
int JsonUtils::GetValue<int>(const rapidjson::Value& value, const char* path)
{
    const rapidjson::Value& subValue = value[path];
    if (subValue.IsInt())
        return subValue.GetInt();
    else
    {  if (subValue.IsString())
            return boost::lexical_cast<int>(subValue.GetString());
        else {
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
template <>
double JsonUtils::GetValue<double>(const rapidjson::Value& value, const char* path)
{
    const rapidjson::Value& subValue = value[path];
    if (subValue.IsDouble())
        return subValue.GetDouble();
    else
    {  if (subValue.IsString())
            return boost::lexical_cast<double>(subValue.GetString());
        else {
            return -1.0;
        }
    }
}


