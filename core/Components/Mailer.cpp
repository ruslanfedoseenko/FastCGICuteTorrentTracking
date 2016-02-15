/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mailer.cpp
 * Author: Ruslan
 * 
 * Created on February 15, 2016, 11:16 PM
 */

#include "Mailer.h"
#include <iostream>
#include <fastcgi2/config.h>
Mailer::Mailer(fastcgi::ComponentContext* componentContext)
: fastcgi::Component(componentContext)
{
    std::string val = context()->getConfig()->asString(context()->getComponentXPath() + "/tempVal");
    std::cout << "Mailer Val = "<< val << std::endl;
}

void Mailer::onLoad()
{

}

void Mailer::onUnload()
{

}



Mailer::~Mailer()
{
}

