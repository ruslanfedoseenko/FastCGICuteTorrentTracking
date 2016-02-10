/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HandlerDescriptor.h
 * Author: RuslanF
 *
 * Created on February 9, 2016, 2:57 PM
 */

#ifndef HANDLERDESCRIPTOR_H
#define HANDLERDESCRIPTOR_H

#include "RequestFilters.h"


struct HandlerDescriptor {

    std::list<boost::shared_ptr<RequestFilter>> Filters;
    boost::function<void(fastcgi::Request*,fastcgi::HandlerContext*)> Handler;
    

};

#endif /* HANDLERDESCRIPTOR_H */

