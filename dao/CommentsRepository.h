/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CommentsRepository.h
 * Author: Ruslan
 *
 * Created on January 8, 2016, 4:42 PM
 */

#ifndef COMMENTSREPOSITORY_H
#define COMMENTSREPOSITORY_H
#include "../Comments.h"
#include <vector>
#include <iostream>
#include <stlcache/stlcache.hpp>
#include <stlcache/policy_adaptive.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include "RepositoryContext.h"
#include "BaseRepository.h"
#define PAGE_SIZE 30
class CommentsRepository : BaseRepository {
    typedef stlcache::cache<std::string, std::vector<Comment>, stlcache::policy_adaptive> CacheAdaptive;
public:
    CommentsRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword);
    std::vector<Comment> GetComments(std::string infoHash, int page, boost::shared_ptr<RepositoryContext> context = nullptr);
    int GetCommentsPageCount(std::string infoHash, boost::shared_ptr<RepositoryContext> context = nullptr);
    float GetCommentsAvarageRating(std::string infoHash, boost::shared_ptr<RepositoryContext> context = nullptr);
    void AddComments(const std::vector<Comment>& comments, boost::shared_ptr<RepositoryContext> context = nullptr);
    virtual ~CommentsRepository();
private:
    CacheAdaptive m_commentsCache;
    boost::mutex m_readMutex;
    
};

#endif /* COMMENTSREPOSITORY_H */
