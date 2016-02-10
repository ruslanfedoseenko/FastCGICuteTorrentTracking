/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NewUsersRepository.cpp
 * Author: Ruslan
 * 
 * Created on February 9, 2016, 11:00 PM
 */

#include "NewUsersRepository.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>  
#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#define SESSION_EXPIRE_IN 2400

NewUsersRepository::NewUsersRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword)
: BaseRepository(dbHost, dbUser, dbPassword)
{
}

std::string NewUsersRepository::AddUser(const User& user, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    std::string authToken = GenerateAuthToken();
    int currentAuthTokenId = AddAuthToken(authToken, context);
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> insertUserStatment(connection->prepareStatement("INSERT INTO `users`(`user_id`, `userName`, `mail`, `password`, `token_id`) VALUES ( ?, ?, ?, ?, ?)"));
    int index = 0;
    insertUserStatment->setString(index++, user.userId);
    insertUserStatment->setString(index++, user.userName);
    insertUserStatment->setString(index++, user.email);
    insertUserStatment->setString(index++, user.password);
    insertUserStatment->setInt(index++, currentAuthTokenId);
    insertUserStatment->execute();
    return authToken;
}

User NewUsersRepository::GetUser(const std::string& userId, boost::shared_ptr<RepositoryContext> context )
{
    if (context == nullptr)
    {
	context = createContext();
    }
    User result;
    result.userId = userId;
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> getUserStatment(connection->prepareStatement("SELECT u.`userName` , u.`mail` , u.`password` , at.token FROM  `users` AS u LEFT JOIN auth_tokens AS at ON u.token_id = at.id AND at.is_enabled =  TRUE AND TIMESTAMPDIFF( SECOND , at.`last_used` , CURRENT_TIMESTAMP ) < ? WHERE u.`user_id` =  ?"));
    getUserStatment->setInt(0, SESSION_EXPIRE_IN);
    getUserStatment->setString(1, userId);
    boost::scoped_ptr<sql::ResultSet> userResult(getUserStatment->executeQuery());
    while (userResult->next())
    {
	int index = 0;
	result.userName = userResult->getString(index++);
	result.email = userResult->getString(index++);
	result.password = userResult->getString(index++);
	if (!userResult->isNull(index++))
	{
	    result.authToken = userResult->getString(index);
	}
    }

    return result;
}

std::string NewUsersRepository::Authentificate(const std::string& login, const std::string& password, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> findUserStatment(connection->prepareStatement("SELECT COUNT(*), `token_id`, `user_id` FROM `users`  WHERE `userName` = ? AND `password` = ? LIMIT 0,1"));
    findUserStatment->setString(0, login);
    findUserStatment->setString(1, password);
    boost::scoped_ptr<sql::ResultSet> findUserResult(findUserStatment->executeQuery());
    while(findUserResult->next())
    {
	int32_t userCount = findUserResult->getInt(0);
	if (userCount > 0)
	{
	    if (userCount > 1)
	    {
		std::cout << "Warning more than one user found for data" << login << password;
	    }
	    int32_t tokenId = findUserResult->getInt(1);
	    std::string userId = findUserResult->getString(2);
	    DeactivateToken(tokenId, context);
	    std::string authToken = GenerateAuthToken();
	    int currentAuthTokenId = AddAuthToken(authToken, context);
	    boost::scoped_ptr<sql::PreparedStatement> updateTokenStatment(connection->prepareStatement("UPDATE `users` SET `token_id`=? WHERE `user_id` = ?"));
	    updateTokenStatment->setInt(0, currentAuthTokenId);
	    updateTokenStatment->setString(1, userId);
	    return authToken;
	}
    }
    return "";
}

bool NewUsersRepository::CheckAlive(const std::string& userId, const std::string& authToken, boost::shared_ptr<RepositoryContext> context)
{
     if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> findUserStatment(connection->prepareStatement("SELECT COUNT(*) FROM `users` as u LEFT JOIN auth_tokens as at WHERE u.`user_id` = ? AND at.`token` = ? AND at.`is_enabled` = TRUE AND TIMESTAMPDIFF( SECOND , at.`last_used` , CURRENT_TIMESTAMP ) < ?"));
    int index = 0;
    findUserStatment->setString(index++, userId);
    findUserStatment->setString(index++, authToken);
    findUserStatment->setInt(index++, SESSION_EXPIRE_IN);
    boost::scoped_ptr<sql::ResultSet> findUserResult(findUserStatment->executeQuery());
    while (findUserResult->next())
    {
	return findUserResult->getInt(0) > 0;
    }
    return false;
}


int NewUsersRepository::AddAuthToken(std::string token, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> inserStatment(connection->prepareStatement("INSERT INTO `auth_tokens`(`token` ) VALUES( ? )"));
    inserStatment->setString(0, token);
    inserStatment->execute();
    boost::scoped_ptr<sql::PreparedStatement> getLastId(connection->prepareStatement("SELECT LAST_INSERT_ID() FROM `auth_tokens`"));
    boost::scoped_ptr<sql::ResultSet> result(getLastId->executeQuery());
    int insertId = -1;
    while (result->next())
    {
	insertId = result->getInt(0);
    }
    return insertId;
}

void NewUsersRepository::DeactivateToken(int id, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> deactivateStatment(connection->prepareStatement("UPDATE `auth_tokens` SET `is_enabled`=FALSE WHERE `id`=?"));
    deactivateStatment->setInt(0, id);
    int affectedCount = deactivateStatment->executeUpdate();
    if (affectedCount == 0)
    {
	std::cout << "No auth token found for id " << id;
    }
}

std::string NewUsersRepository::GenerateAuthToken()
{
    time_t seconds;
    time(&seconds);
    std::string str = std::to_string(seconds);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
	ss << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];
    }
    return ss.str();
}

