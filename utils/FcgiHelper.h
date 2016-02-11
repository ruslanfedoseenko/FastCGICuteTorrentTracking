/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FcgiHelper.h
 * Author: Ruslan
 *
 * Created on February 11, 2016, 1:13 AM
 */

#ifndef FCGIHELPER_H
#define FCGIHELPER_H
#include <string>
namespace fastcgi{
    class Request;
}
#include <rapidjson/document.h>
#include <rapidjson/error/error.h>
class FcgiHelper {
public:
    static void WriteParseError(fastcgi::Request* request, rapidjson::ParseErrorCode err);
    static void WriteError(fastcgi::Request* request, int error_code, std::string message);
    static void WriteJson(fastcgi::Request* request, const rapidjson::Document& doc);
private:

};

#endif /* FCGIHELPER_H */

