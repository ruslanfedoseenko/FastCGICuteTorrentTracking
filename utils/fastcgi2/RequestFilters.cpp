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
#include <fastcgi2/handler.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <fastcgi2/handler.h>

RegexFilter::RegexFilter(const std::string &regex) : regex_(regex)
{
}

RegexFilter::~RegexFilter()
{
}

class namesaving_smatch : public boost::smatch
{
public:

    namesaving_smatch(const boost::regex& pattern)
    {
	std::string pattern_str = pattern.str();
	boost::regex capture_pattern("\\?P?<(\\w+)>");
	auto words_begin = boost::sregex_iterator(pattern_str.begin(), pattern_str.end(), capture_pattern);
	auto words_end = boost::sregex_iterator();

	for (boost::sregex_iterator i = words_begin; i != words_end; i++)
	{
	    std::string name = (*i)[1].str();
	    m_names.push_back(name);
	}
    }

    ~namesaving_smatch()
    {
    }

    std::vector<std::string>::const_iterator names_begin() const
    {
	return m_names.begin();
    }

    std::vector<std::string>::const_iterator names_end() const
    {
	return m_names.end();
    }

private:
    std::vector<std::string> m_names;
} ;

bool
RegexFilter::check(const std::string &value)
{
    vraibles.clear();
    namesaving_smatch what(regex_);
    bool r = boost::regex_match(value, what, regex_);
    if (r)
    {
	boost::smatch::const_iterator i = what.begin();
	for (auto it = what.names_begin(); it != what.names_end(); ++it)
	{
	    std::string name = *it;
	    std::string value = what[*it].str();
	    vraibles.insert({name, value});
	}
    }

    return r;
}

void RegexFilter::FillExtractedVariables(fastcgi::HandlerContext* handlerContext)
{
    if (handlerContext == nullptr)
	return;
    for (auto i = vraibles.begin(); i != vraibles.end(); ++i)
    {
	handlerContext->setParam(i->first, i->second);
    }
}

RequestTypeFilter::RequestTypeFilter(const std::string& type)
: m_type(type)
{

}

bool RequestTypeFilter::check(const fastcgi::Request *request)
{
    return boost::iequals(request->getRequestMethod(), m_type);
}

UrlFilter::UrlFilter(const std::string &regex) : regex_(regex)
{
}

UrlFilter::~UrlFilter()
{
}

bool
UrlFilter::check(const fastcgi::Request *request)
{
    return regex_.check(request->getScriptName());
}

HostFilter::HostFilter(const std::string &regex) : regex_(regex)
{
}

HostFilter::~HostFilter()
{
}

bool
HostFilter::check(const fastcgi::Request *request)
{
    return regex_.check(request->getHost());
}

PortFilter::PortFilter(const std::string &regex) : regex_(regex)
{
}

PortFilter::~PortFilter()
{
}

bool
PortFilter::check(const fastcgi::Request *request)
{
    std::string port = boost::lexical_cast<std::string>(request->getServerPort());
    return regex_.check(port);
}

AddressFilter::AddressFilter(const std::string &regex) : regex_(regex)
{
}

AddressFilter::~AddressFilter()
{
}

bool
AddressFilter::check(const fastcgi::Request *request)
{
    return regex_.check(request->getServerAddr());
}

RefererFilter::RefererFilter(const std::string &regex) : regex_(regex)
{
}

RefererFilter::~RefererFilter()
{
}

bool
RefererFilter::check(const fastcgi::Request *request)
{
    if (!request->hasHeader("Referer"));

    return regex_.check(request->getHeader("Referer"));
}

ParamFilter::ParamFilter(const std::string &name, const std::string &regex) :
name_(name), regex_(regex)
{
}

ParamFilter::~ParamFilter()
{
}

bool
ParamFilter::check(const fastcgi::Request *request)
{
    if (!request->hasArg(name_))
    {
	return false;
    }
    return regex_.check(request->getArg(name_));
}


