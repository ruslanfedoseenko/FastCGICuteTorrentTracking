/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RequestFilters.cpp
 * Author: RuslanF
 * 
 * Created on February 9, 2016, 2:52 PM
 */

#include "RequestFilters.h"
#include <fastcgi2/request.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
RegexFilter::RegexFilter(const std::string &regex) : regex_(regex)
{}

RegexFilter::~RegexFilter()
{}

bool
RegexFilter::check(const std::string &value) const {
    return boost::regex_match(value, regex_);
}

RequestTypeFilter::RequestTypeFilter(const std::string& type) 
: m_type(type)
{

}

bool RequestTypeFilter::check(const fastcgi::Request* request) const
{
    return boost::iequals(request->getRequestMethod(), m_type);
}

UrlFilter::UrlFilter(const std::string &regex) : regex_(regex)
{}

UrlFilter::~UrlFilter()
{}

bool
UrlFilter::check(const fastcgi::Request *request) const {
    return regex_.check(request->getScriptName());
}

HostFilter::HostFilter(const std::string &regex) : regex_(regex)
{}

HostFilter::~HostFilter()
{}

bool
HostFilter::check(const fastcgi::Request *request) const {
    return regex_.check(request->getHost());
}


PortFilter::PortFilter(const std::string &regex) : regex_(regex)
{}

PortFilter::~PortFilter()
{}

bool
PortFilter::check(const fastcgi::Request *request) const {
    std::string port = boost::lexical_cast<std::string>(request->getServerPort());
    return regex_.check(port);
}


AddressFilter::AddressFilter(const std::string &regex) : regex_(regex)
{}

AddressFilter::~AddressFilter()
{}

bool
AddressFilter::check(const fastcgi::Request *request) const {
    return regex_.check(request->getServerAddr());
}

RefererFilter::RefererFilter(const std::string &regex) : regex_(regex)
{}

RefererFilter::~RefererFilter()
{}

bool
RefererFilter::check(const fastcgi::Request *request) const {
    if (!request->hasHeader("Referer"));

    return regex_.check(request->getHeader("Referer"));
}

ParamFilter::ParamFilter(const std::string &name, const std::string &regex) :
        name_(name), regex_(regex)
{}

ParamFilter::~ParamFilter()
{}

bool
ParamFilter::check(const fastcgi::Request *request) const {
    if (!request->hasArg(name_)) {
        return false;
    }
    return regex_.check(request->getArg(name_));
}


