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
    int parentCommentId, id;
    std::string comment, infohash, userToken, comentTime;
    float rating;
    std::vector<int> childComments;
};

#endif /* COMMENTS_H */

