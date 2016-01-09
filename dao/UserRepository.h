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
class UserRepository : BaseRepository{
public:
    UserRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword);
    void SetUsersOnline(const std::vector<std::string>& userTokens, boost::shared_ptr<RepositoryContext> context = nullptr);
    void SetUserName(const std::string& userToken, const std::string& name, boost::shared_ptr<RepositoryContext> context = nullptr);
    std::string GetUserName(const std::string& userToken, boost::shared_ptr<RepositoryContext> context = nullptr);
    virtual ~UserRepository();
private:
    

};

#endif /* USERREPOSITORY_H */

