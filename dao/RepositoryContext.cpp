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
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include "RepositoryContext.h"

RepositoryContext::RepositoryContext(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword, const std::string& dbName)
: m_dbHost(dbHost)
, m_dbUser(dbUser)
, m_dbPassword(dbPassword)
, m_dbName(dbName)
{
}

boost::shared_ptr<sql::Connection> RepositoryContext::GetConnection()
{
    if (m_dbConnection == nullptr)
    {
	sql::Driver* driver = get_driver_instance();
	driver->threadInit();
	m_dbConnection.reset(driver->connect(m_dbHost, m_dbUser, m_dbPassword));
	m_dbConnection->setSchema(m_dbName);
	//        m_dbConnection->setAutoCommit(false);
	//        m_pSavePoint = m_dbConnection->setSavepoint();

    }
    return m_dbConnection;
}

void RepositoryContext::rollback()
{
    if (m_dbConnection != nullptr)
    {
	//        if (m_pSavePoint!= 0)
	//	{
	//            m_dbConnection->rollback(m_pSavePoint);
	//        }
    }
}

RepositoryContext::~RepositoryContext()
{
    //    m_dbConnection->releaseSavepoint(m_pSavePoint);
    //    m_dbConnection->commit();
    sql::Driver* driver = get_driver_instance();
    driver->threadEnd();
    m_dbConnection->close();
}

