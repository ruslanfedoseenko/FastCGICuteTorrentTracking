/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UserRepository.cpp
 * Author: Ruslan
 * 
 * Created on January 9, 2016, 1:05 PM
 */

#include "UserRepository.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/driver.h>
#include <vector>

UserRepository::UserRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword)
: BaseRepository(dbHost, dbUser, dbPassword)
{
}

long UserRepository::GetOnlineUsersCount(boost::shared_ptr<RepositoryContext> context)
{
    long onlineCount = -1;
    try
    {
	if (context == nullptr)
	{
	    context = createContext();
	}
	boost::shared_ptr<sql::Connection> connection = context->GetConnection();
	boost::scoped_ptr<sql::PreparedStatement> stmt(connection->prepareStatement("SELECT COUNT( * ) as online_count FROM  `online` WHERE TIMESTAMPDIFF( SECOND ,  `last_seen` , CURRENT_TIMESTAMP ) <2400"));
	boost::scoped_ptr<sql::ResultSet> onlineResultSet(stmt->executeQuery());

	while (onlineResultSet->next())
	{
	    onlineCount = onlineResultSet->getInt("online_count");
	}
    }
    catch (sql::SQLException ex)
    {
	std::cout << "SQLException occured: " << ex.what() << std::endl;
    }

    return onlineCount;

}

void UserRepository::SetUsersOnline(const std::unordered_map<std::string, std::string> userTokens, boost::shared_ptr<RepositoryContext> context)
{
    if (userTokens.size() == 0)
	return;
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    std::string query = "INSERT INTO `online`(`user_id`, `last_seen`) VALUES ";
    for (int i=0; i < userTokens.size(); i++)
    {
	query.append("(?, ?) ,");
    }
    query.erase(query.length() - 1);
    query.append(" ON DUPLICATE KEY UPDATE `last_seen`= VALUES(`last_seen`)");
    std::cout << "Query: " << query << std::endl;
    boost::scoped_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
    int index = 1;
    for (std::unordered_map<std::string, std::string>::const_iterator i = userTokens.begin(); i != userTokens.end(); ++i)
    {
	stmt->setString(index++, (*i).first);
	stmt->setString(index++, (*i).second);
    }
    stmt->execute();
}

void UserRepository::SetUsersOnline(const std::vector<std::string>& userTokens, boost::shared_ptr<RepositoryContext> context)
{
    if (userTokens.size() == 0)
	return;
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    std::string query = "INSERT INTO `online`(`user_id`) VALUES ";
    for (int i=0; i < userTokens.size(); i++)
    {
	query.append("(? ) ,");
    }
    query.erase(query.length() - 1);
    query.append(" ON DUPLICATE KEY UPDATE `last_seen`=NOW()");
    std::cout << "Query: " << query << std::endl;
    boost::scoped_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
    int index = 1;
    for (std::vector<std::string>::const_iterator i = userTokens.begin(); i != userTokens.end(); ++i)
    {
	stmt->setString(index++, *i);
    }
    stmt->execute();
}

void UserRepository::SetUserNames(const std::unordered_map<std::string, std::string>& userNames, boost::shared_ptr<RepositoryContext> context)
{
    if (userNames.size() == 0)
	return;
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    std::string query = "INSERT INTO `online`(`user_id`, `userName`) VALUES ";
    for (int i = 0 ; i < userNames.size(); i++)
    {
	query.append("(?, ?) ,");
    }
    query.erase(query.length() - 1);
    query.append(" ON DUPLICATE KEY UPDATE `userName`= VALUES(`userName`)");
    boost::scoped_ptr<sql::PreparedStatement> stmt(connection->prepareStatement(query));
    int index = 1;
    for (std::unordered_map<std::string, std::string>::const_iterator i = userNames.begin(); i!= userNames.end(); ++i)
    {
	stmt->setString(index++, (*i).first);
	stmt->setString(index++, (*i).second);
    }
    stmt->execute();
    
}

std::string  UserRepository::GetUserName(const std::string& userToken, boost::shared_ptr<RepositoryContext> context)
{
    std::string userName;
    try
    {
	if (context == nullptr)
	{
	    context = createContext();
	}
	boost::shared_ptr<sql::Connection> connection = context->GetConnection();
	boost::scoped_ptr<sql::PreparedStatement> stmt(connection->prepareStatement("SELECT `userName` FROM `online` WHERE `user_id` = LOWER(?)"));
	stmt->setString(1, userToken);
	boost::scoped_ptr<sql::ResultSet> onlineResultSet(stmt->executeQuery());

	while (onlineResultSet->next())
	{
	    userName = onlineResultSet->getString("userName");
	}
    }
    catch (sql::SQLException ex)
    {
	std::cout << "SQLException occured: " << ex.what() << std::endl;
    }
    return userName;
}

UserRepository::~UserRepository()
{
}

