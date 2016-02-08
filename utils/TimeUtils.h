/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TimeUtils.h
 * Author: Ruslan
 *
 * Created on January 9, 2016, 6:38 PM
 */

#ifndef TIMEUTILS_H
#define TIMEUTILS_H
#include <string>
class TimeUtils {
public:
    static std::string ToSqlString(time_t time);
private:

};

#endif /* TIMEUTILS_H */

