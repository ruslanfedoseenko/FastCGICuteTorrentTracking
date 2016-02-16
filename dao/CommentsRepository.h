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
#include "Comments.h"
#include <vector>
#include <iostream>
#include <stlcache/stlcache.hpp>
#include <stlcache/policy_adaptive.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include "RepositoryContext.h"
#include "BaseRepository.h"
#include <fastcgi2/component.h>
namespace fastcgi {
    class ComponentContext;
}
#define PAGE_SIZE 30
class NewUsersRepository;

class CommentsRepository : BaseRepository, public fastcgi::Component {
    typedef stlcache::cache<uint32_t, std::vector<Comment>, stlcache::policy_adaptive> CacheAdaptive;
public:
    void onLoad();
    void onUnload();
    CommentsRepository(fastcgi::ComponentContext* componentContext);
    std::vector<Comment> GetComments(std::string infoHash, int page, boost::shared_ptr<RepositoryContext> context = nullptr);
    int GetCommentsPageCount(std::string infoHash, boost::shared_ptr<RepositoryContext> context = nullptr);
    float GetCommentsAvarageRating(std::string infoHash, boost::shared_ptr<RepositoryContext> context = nullptr);
    void AddComments(const std::vector<Comment>& comments, boost::shared_ptr<RepositoryContext> context = nullptr);
    bool CheckCommentToken(std::string token, int comment_id, boost::shared_ptr<RepositoryContext> context = nullptr);
    virtual ~CommentsRepository();
private:
    boost::scoped_ptr<NewUsersRepository> m_pAuthRepo;
    CacheAdaptive m_commentsCache;
    boost::mutex m_readMutex;

};

#endif /* COMMENTSREPOSITORY_H */

