/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Comments.h
 * Author: Ruslan
 *
 * Created on January 8, 2016, 4:38 PM
 */

#ifndef COMMENTS_H
#define COMMENTS_H
#include <string>
#include <vector>

struct Comment {
    Comment()
    {
        id = 0;
        rating = 0.0f;
        canEdit = false;
    }
    int id;
    bool canEdit;
    std::string comment, infohash, userToken, comentAddDateTime, comentUpdateDateTime;
    float rating;
};

#endif /* COMMENTS_H */

