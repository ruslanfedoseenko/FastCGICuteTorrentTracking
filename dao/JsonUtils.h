/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   JsonUtils.h
 * Author: Ruslan
 *
 * Created on February 8, 2016, 11:59 PM
 */

#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <fastcgi2/data_buffer.h>
#include <boost/lexical_cast.hpp>

class JsonUtils {
public:
    static bool ParseJson(rapidjson::Document& document, fastcgi::DataBuffer& buffer);
    template <class T> static T GetValue(const rapidjson::Value& document, const char* path);
private:

};

#endif /* JSONUTILS_H */

