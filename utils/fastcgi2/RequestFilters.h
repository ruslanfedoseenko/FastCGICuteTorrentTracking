/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RequestFilters.h
 * Author: RuslanF
 *
 * Created on February 9, 2016, 2:52 PM
 */

#ifndef REQUESTFILTERS_H
#define REQUESTFILTERS_H

#include <boost/regex.hpp>

namespace fastcgi{
    class Request;
}


class RequestFilter {
public:
    virtual bool check(const fastcgi::Request *request) const = 0;
};
class RequestTypeFilter : public RequestFilter{
public:
    RequestTypeFilter(const std::string &type);
    bool check(const fastcgi::Request *request) const;
private:
    std::string m_type;
};

class RegexFilter  {
public:
    RegexFilter(const std::string &regex);
    ~RegexFilter();

    bool check(const std::string &value) const;
private:
    boost::regex regex_;
};

class UrlFilter : public RequestFilter {
public:
    UrlFilter(const std::string &regex);
    ~UrlFilter();

    virtual bool check(const fastcgi::Request *request) const;
private:
    RegexFilter regex_;
};

class HostFilter : public RequestFilter {
public:
    HostFilter(const std::string &regex);
    ~HostFilter();

    virtual bool check(const fastcgi::Request *request) const;
private:
    RegexFilter regex_;
};

class PortFilter : public RequestFilter {
public:
    PortFilter(const std::string &regex);
    ~PortFilter();

    virtual bool check(const fastcgi::Request *request) const;
private:
    RegexFilter regex_;
};

class AddressFilter : public RequestFilter {
public:
    AddressFilter(const std::string &regex);
    ~AddressFilter();

    virtual bool check(const fastcgi::Request *request) const;
private:
    RegexFilter regex_;
};

class RefererFilter : public RequestFilter {
public:
    RefererFilter(const std::string &regex);
    ~RefererFilter();

    virtual bool check(const fastcgi::Request *request) const;
private:
    RegexFilter regex_;
};

class ParamFilter : public RequestFilter {
public:
    ParamFilter(const std::string &name, const std::string &regex);
    ~ParamFilter();

    virtual bool check(const fastcgi::Request *request) const;
private:
    std::string name_;
    RegexFilter regex_;
};


#endif /* REQUESTFILTERS_H */

