/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VariableExtractor.cpp
 * Author: RuslanF
 * 
 * Created on February 10, 2016, 4:45 PM
 */

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "VariableExtractor.h"

template<class TVar>
TVar VariableExtractor::Extract(boost::smatch match, const std::string& vraibleName)
{
    if (match.position(vraibleName) > 0)
    {
        return boost::lexical_cast<TVar>(match[vraibleName]);
    }
    return TVar();
}


VariableExtractor::~VariableExtractor()
{

}

