/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Subrouter.cpp
 * Author: RuslanF
 * 
 * Created on February 9, 2016, 2:04 PM
 */

#include "Subrouter.h"
#include <fastcgi2/request.h>
#include <boost/algorithm/string.hpp>

HandlerDescriptor* Subrouter::RegisterHandler(boost::function<void(fastcgi::Request*, fastcgi::HandlerContext*) > handler)
{
    HandlerDescriptor* descriptor = new HandlerDescriptor;
    descriptor->Handler = handler;
    m_rules.push_back(descriptor);
    return descriptor;
}

bool Subrouter::HandleRequest(fastcgi::Request* request,  fastcgi::HandlerContext *handlerContext)
{
    for (std::vector<HandlerDescriptor*>::const_iterator i = m_rules.begin(); i != m_rules.end(); ++i )
    {
	HandlerDescriptor* decriptor = *i;
	std::list<boost::shared_ptr < RequestFilter>> filters = decriptor->Filters;
	bool match = filters.size() > 0;
	for (std::list<boost::shared_ptr < RequestFilter>>::const_iterator j = filters.begin(); j != filters.end(); ++j)
	{
	    boost::shared_ptr<RequestFilter> filter = *j;
	    match = match && filter->check(request);
	    filter->fillVars(handlerContext);
	    if (!match)
	    {
		break;
	    }
	}
	if (match)
	{
	    decriptor->Handler(request, handlerContext);
	    return true;
	}
    }
    std::stringbuf buffer("{\"state\" : \"error\", \"errorString\" : \"No handler found.\"}");
    request->setStatus(400);
    request->write(&buffer);
    return false;
}

Subrouter::~Subrouter()
{
    for (int i=0; i < m_rules.size(); i++)
    {
	delete m_rules[i];
    }
    m_rules.clear();
}