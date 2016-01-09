/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BaseRepository.cpp
 * Author: Ruslan
 * 
 * Created on January 9, 2016, 1:12 PM
 */

#include "BaseRepository.h"

BaseRepository::BaseRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword)
    : m_dbHost(dbHost)
    , m_dbUser(dbUser)
    , m_dbPassword(dbPassword)
{
}

boost::shared_ptr<RepositoryContext> BaseRepository::createContext()
{
    boost::shared_ptr<RepositoryContext> context(new RepositoryContext(m_dbHost, m_dbUser, m_dbPassword));
    return context;
}

BaseRepository::~BaseRepository()
{
}

