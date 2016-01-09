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
#include <cppconn/driver.h>
#include <vector>
UserRepository::UserRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword)
: BaseRepository(dbHost, dbUser, dbPassword)
{
}

void UserRepository::SetUsersOnline(const std::vector<std::string>& userTokens, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    std::string query = "INSERT INTO `online`(`user_id`) VALUES ";
    for (int i=0; i < userTokens.size(); i++)
    {
	query.append("( ? ) ,");
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

void UserRepository::SetUserName(const std::string& userToken, const std::string& name, boost::shared_ptr<RepositoryContext> context)
{

}

std::string  UserRepository::GetUserName(const std::string& userToken, boost::shared_ptr<RepositoryContext> context)
{
    std::string userName;
    return userName;
}

UserRepository::~UserRepository()
{
}

