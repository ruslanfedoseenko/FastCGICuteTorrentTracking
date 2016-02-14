/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   User.h
 * Author: Ruslan
 *
 * Created on February 9, 2016, 10:44 PM
 */

#ifndef USER_H
#define USER_H

struct User {
    User()
    {
        id = 0;
        lastSeen = 0;
    }
    std::string userName, email, authToken, password;
    unsigned lastSeen, id;
};

#endif /* USER_H */

