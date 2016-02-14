/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HashUtils.h
 * Author: Ruslan
 *
 * Created on February 14, 2016, 7:31 PM
 */

#ifndef HASHUTILS_H
#define HASHUTILS_H
#include <boost/cstdint.hpp>
#include <string>
class HashUtils {
public:
    static uint32_t CalculateCrc32(const std::string& data);
private:

};

#endif /* HASHUTILS_H */

