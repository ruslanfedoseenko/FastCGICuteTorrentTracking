/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UserRepository.h
 * Author: Ruslan
 *
 * Created on January 9, 2016, 1:05 PM
 */

#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "BaseRepository.h"
#include <boost/smart_ptr.hpp>
#include <unordered_map>
#include <fastcgi2/component.h>
class UserRepository : BaseRepository, public fastcgi::Component {
public:
    UserRepository(fastcgi::ComponentContext*);
    void onLoad() {}
    void onUnload() {}
    long GetOnlineUsersCount(boost::shared_ptr<RepositoryContext> context = nullptr);
    void SetUsersOnline(const std::vector<std::string>& userTokens, boost::shared_ptr<RepositoryContext> context = nullptr);
    void SetUsersOnline(const std::unordered_map<std::string, std::string> userTokens, boost::shared_ptr<RepositoryContext> context = nullptr);
    void SetUserNames(const std::unordered_map<std::string, std::string>& userNames, boost::shared_ptr<RepositoryContext> context = nullptr);
    std::string GetUserName(const std::string& userToken, boost::shared_ptr<RepositoryContext> context = nullptr);
    virtual ~UserRepository();
private:


};

#endif /* USERREPOSITORY_H */

