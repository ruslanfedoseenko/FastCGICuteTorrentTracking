/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HashUtils.cpp
 * Author: Ruslan
 * 
 * Created on February 14, 2016, 7:31 PM
 */

#include "HashUtils.h"
#include <boost/crc.hpp> 
uint32_t HashUtils::CalculateCrc32(const std::string& data)
{
    boost::crc_32_type crc32_result;
    crc32_result.process_bytes(data.c_str(), data.length());
    return crc32_result.checksum();
}


