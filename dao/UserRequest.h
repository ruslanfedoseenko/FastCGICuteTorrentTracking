/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UserRequest.h
 * Author: Ruslan
 *
 * Created on January 9, 2016, 6:22 PM
 */

#ifndef USERREQUEST_H
#define USERREQUEST_H
#include <time.h>
#include <string>
#include <boost/variant.hpp>

struct UserRequest {

    enum RequestType {
        HeartBeat,
        UserNameUpdate
    };
    RequestType type;
    std::string userToken;
    boost::variant<std::string, time_t> value;
};

#endif /* USERREQUEST_H */

