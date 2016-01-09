/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BaseRepository.h
 * Author: Ruslan
 *
 * Created on January 9, 2016, 1:12 PM
 */

#ifndef BASEREPOSITORY_H
#define BASEREPOSITORY_H
#include "RepositoryContext.h"
#include <boost/smart_ptr.hpp>
class BaseRepository {
public:
    BaseRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword);
    virtual ~BaseRepository();
private:
    std::string m_dbHost, m_dbUser, m_dbPassword;
protected:
    boost::shared_ptr<RepositoryContext> createContext();

};

#endif /* BASEREPOSITORY_H */

