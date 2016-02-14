/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Errors.h
 * Author: Ruslan
 *
 * Created on February 11, 2016, 12:30 AM
 */

#ifndef ERRORS_H
#define ERRORS_H

enum Error {
    None = 0,
    InvalidJson,
    RequiredDataMissing,
    TokenExpired,
    InvalidPageNumber
};

#endif /* ERRORS_H */

