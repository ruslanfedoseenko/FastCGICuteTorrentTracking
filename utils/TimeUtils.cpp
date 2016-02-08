/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TimeUtils.cpp
 * Author: Ruslan
 * 
 * Created on January 9, 2016, 6:38 PM
 */

#include "TimeUtils.h"

std::string TimeUtils::ToSqlString(time_t rawTime)
{
    struct tm * timeinfo;
    char buffer[80];
    timeinfo = localtime(&rawTime);

    strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
    std::string str(buffer);
    return str;
	   
}
