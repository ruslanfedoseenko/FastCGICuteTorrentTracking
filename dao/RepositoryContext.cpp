/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RepositoryContext.cpp
 * Author: Ruslan
 * 
 * Created on January 9, 2016, 10:55 AM
 */

#include <boost/smart_ptr/weak_ptr.hpp>
#include "RepositoryContext.h"

RepositoryContext::RepositoryContext(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword)
    : m_dbHost(dbHost)
    , m_dbUser(dbUser)
    , m_dbPassword(dbPassword)
{
}

boost::shared_ptr<sql::Connection> RepositoryContext::GetConnection()
{
    if (m_dbConnection == nullptr)
    {
	sql::Driver* driver = get_driver_instance();
	driver->threadInit();
	m_dbConnection.reset(driver->connect(m_dbHost, m_dbUser, m_dbPassword));
	m_dbConnection->setSchema("tracking_db");
	m_dbConnection->setAutoCommit(false);
    }
    return m_dbConnection;
}
void RepositoryContext::rollback()
{
    if (m_dbConnection != nullptr)
    {
	m_dbConnection->rollback();
    }
}

RepositoryContext::~RepositoryContext()
{
    m_dbConnection->commit();
    sql::Driver* driver = get_driver_instance();
    driver->threadEnd();
    m_dbConnection->close();
}

