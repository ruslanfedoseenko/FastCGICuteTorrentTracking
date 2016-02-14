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
#include <unordered_map>
namespace fastcgi {
    class Request;
    class HandlerContext;
}

class RequestFilter {
public:
    virtual bool check(const fastcgi::Request *request) = 0;
    virtual void fillVars(fastcgi::HandlerContext* context) = 0;
};

class RequestTypeFilter : public RequestFilter {
public:
    RequestTypeFilter(const std::string &type);
    bool check(const fastcgi::Request *request);

    void fillVars(fastcgi::HandlerContext* context) {

    }
private:
    std::string m_type;
};

class RegexFilter {
public:
    RegexFilter(const std::string &regex);
    ~RegexFilter();
    void FillExtractedVariables(fastcgi::HandlerContext* handlerContext);
    bool check(const std::string &value);
private:
    std::unordered_map<std::string, std::string> vraibles;
    boost::regex regex_;
};

class UrlFilter : public RequestFilter {
public:
    UrlFilter(const std::string &regex);
    ~UrlFilter();

    virtual bool check(const fastcgi::Request *request);

    void fillVars(fastcgi::HandlerContext* context) {
        regex_.FillExtractedVariables(context);
    }
private:
    RegexFilter regex_;
};

class HostFilter : public RequestFilter {
public:
    HostFilter(const std::string &regex);
    ~HostFilter();

    void fillVars(fastcgi::HandlerContext* context) {
        regex_.FillExtractedVariables(context);
    }
    virtual bool check(const fastcgi::Request *request);
private:
    RegexFilter regex_;
};

class PortFilter : public RequestFilter {
public:
    PortFilter(const std::string &regex);
    ~PortFilter();

    void fillVars(fastcgi::HandlerContext* context) {
        regex_.FillExtractedVariables(context);
    }
    virtual bool check(const fastcgi::Request *request);
private:
    RegexFilter regex_;
};

class AddressFilter : public RequestFilter {
public:
    AddressFilter(const std::string &regex);
    ~AddressFilter();

    void fillVars(fastcgi::HandlerContext* context) {
        regex_.FillExtractedVariables(context);
    }
    virtual bool check(const fastcgi::Request *request);
private:
    RegexFilter regex_;
};

class RefererFilter : public RequestFilter {
public:
    RefererFilter(const std::string &regex);
    ~RefererFilter();

    void fillVars(fastcgi::HandlerContext* context) {
        regex_.FillExtractedVariables(context);
    }
    virtual bool check(const fastcgi::Request *request);
private:
    RegexFilter regex_;
};

class ParamFilter : public RequestFilter {
public:
    ParamFilter(const std::string &name, const std::string &regex);
    ~ParamFilter();

    void fillVars(fastcgi::HandlerContext* context) {
        regex_.FillExtractedVariables(context);
    }
    virtual bool check(const fastcgi::Request *request);
private:
    std::string name_;
    RegexFilter regex_;
};


#endif /* REQUESTFILTERS_H */

