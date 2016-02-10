/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NewUsersRepository.h
 * Author: Ruslan
 *
 * Created on February 9, 2016, 11:00 PM
 */

#ifndef NEWUSERSREPOSITORY_H
#define NEWUSERSREPOSITORY_H

#include "BaseRepository.h"
#include "User.h"
class NewUsersRepository : public BaseRepository {
public:
    NewUsersRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword);
    std::string AddUser(const User& user, boost::shared_ptr<RepositoryContext> context = nullptr);
    User GetUser(const std::string& userId, boost::shared_ptr<RepositoryContext> context = nullptr);
    // Return non empty auth token on success
    std::string Authentificate(const std::string& login, const std::string& password, boost::shared_ptr<RepositoryContext> context = nullptr);
    bool CheckAlive(const std::string& userId, const std::string& authToken, boost::shared_ptr<RepositoryContext> context = nullptr);
private:
    std::string GenerateAuthToken();
    int AddAuthToken(std::string token, boost::shared_ptr<RepositoryContext> context = nullptr);
    void DeactivateToken(int id, boost::shared_ptr<RepositoryContext> context = nullptr);
};

#endif /* NEWUSERSREPOSITORY_H */
