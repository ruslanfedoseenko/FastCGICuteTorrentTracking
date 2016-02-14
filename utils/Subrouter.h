/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Subrouter.h
 * Author: RuslanF
 *
 * Created on February 9, 2016, 2:04 PM
 */

#ifndef SUBROUTER_H
#define SUBROUTER_H
#include <boost/regex.hpp>
#include <boost/function.hpp>
#include "RequestFilters.h"
#include "HandlerDescriptor.h"
namespace fastcgi {
    class Request;
}

class Subrouter {
public:
    ~Subrouter();
    HandlerDescriptor* RegisterHandler(boost::function<void(fastcgi::Request*, fastcgi::HandlerContext *) > handler);
    bool HandleRequest(fastcgi::Request*, fastcgi::HandlerContext*);

private:

    std::vector<HandlerDescriptor*> m_rules;

};

#endif /* SUBROUTER_H */

