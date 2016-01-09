/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RepositoryContext.h
 * Author: Ruslan
 *
 * Created on January 9, 2016, 10:55 AM
 */

#ifndef REPOSITORYCONTEXT_H
#define REPOSITORYCONTEXT_H
#include <boost/smart_ptr.hpp>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
class RepositoryContext {
public:
    RepositoryContext(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword);
    boost::shared_ptr<sql::Connection> GetConnection();
    virtual ~RepositoryContext();
private:
    std::string m_dbHost, m_dbUser, m_dbPassword;
    boost::shared_ptr<sql::Connection> m_dbConnection;
};

#endif /* REPOSITORYCONTEXT_H */

