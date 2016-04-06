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
#include <fastcgi2/logger.h>
#include <fastcgi2/config.h>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <chrono>

NewUsersRepository::NewUsersRepository(fastcgi::ComponentContext* componentContext)
: BaseRepository()
, fastcgi::Component(componentContext)
, m_sessionExpartionTime(0)
{
    std::string rootXPath = context()->getComponentXPath();
    setDbHost(context()->getConfig()->asString(rootXPath + "/mysqlhost"));
    setDbName(context()->getConfig()->asString(rootXPath + "/mysqldbname"));
    setDbUser(context()->getConfig()->asString(rootXPath + "/mysqluser"));
    setDbPassword(context()->getConfig()->asString(rootXPath + "/mysqlpass"));
    std::string exparationTime = context()->getConfig()->asString(rootXPath + "/sessionexparationtime");
    std::cout << "Session expiration time:" << exparationTime << std::endl;
    m_sessionExpartionTime = boost::lexical_cast<int>(exparationTime);
}

std::string NewUsersRepository::AddUser(const User& user, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
        context = createContext();
    }
    if (!IsMailUnique(user.email, context))
    {
        throw std::logic_error("User with this email already exists");
    }
    std::string authToken = GenerateAuthToken();
    int currentAuthTokenId = AddAuthToken(authToken, context);
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> insertUserStatment(connection->prepareStatement("INSERT INTO `users`(`id`, `userName`, `mail`, `password`, `token_id`) VALUES ( ?, ?, ?, ?, ?)"));
    int index = 1;
    insertUserStatment->setInt(index++, user.id);
    insertUserStatment->setString(index++, user.userName);
    insertUserStatment->setString(index++, user.email);
    insertUserStatment->setString(index++, user.password);
    insertUserStatment->setInt(index++, currentAuthTokenId);
    insertUserStatment->execute();
    return authToken;
}

int NewUsersRepository::GetUserIdByActiveToken(const std::string& authToken, boost::shared_ptr<RepositoryContext> context)
{
    //SELECT u.`id` FROM `users` as u LEFT JOIN auth_tokens as at ON u.`token_id`= at.`id`  WHERE at.`token` = ?
    if (context == nullptr)
    {
        context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> getUserIdStatment(connection->prepareStatement("SELECT u.`id` FROM `users` as u LEFT JOIN auth_tokens as at ON u.`token_id`= at.`id`  WHERE at.`token` = ? "));
    getUserIdStatment->setString(1, authToken);
    boost::scoped_ptr<sql::ResultSet> userIdResult(getUserIdStatment->executeQuery());
    int userId = -1;
    while (userIdResult->next())
    {
        userId = userIdResult->getInt(1);
    }
    return userId;
}

User NewUsersRepository::GetUser(const unsigned& userId, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
        context = createContext();
    }
    User result;
    result.id = userId;
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> getUserStatment(connection->prepareStatement("SELECT u.`userName` , u.`mail` , u.`password` , at.token FROM  `users` AS u LEFT JOIN auth_tokens AS at ON u.token_id = at.id AND at.is_enabled =  TRUE WHERE u.`id` =  ? AND TIMESTAMPDIFF( SECOND , at.`last_used` , CURRENT_TIMESTAMP ) < ? "));
    getUserStatment->setInt(1, m_sessionExpartionTime);
    getUserStatment->setInt(2, userId);
    boost::scoped_ptr<sql::ResultSet> userResult(getUserStatment->executeQuery());
    while (userResult->next())
    {
        int index = 1;
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
    boost::scoped_ptr<sql::PreparedStatement> findUserStatment(connection->prepareStatement("SELECT COUNT(*), `token_id`, `id` FROM `users`  WHERE `userName` = ? AND `password` = ? LIMIT 0,1"));
    findUserStatment->setString(1, login);
    findUserStatment->setString(2, password);
    boost::scoped_ptr<sql::ResultSet> findUserResult(findUserStatment->executeQuery());
    while (findUserResult->next())
    {
        int32_t userCount = findUserResult->getInt(1);
        if (userCount > 0)
        {
            if (userCount > 1)
            {
                std::cout << "Warning more than one user found for data" << login << password;
            }
            int32_t tokenId = findUserResult->getInt(2);
            int32_t userId = findUserResult->getInt(3);
            DeactivateToken(tokenId, context);
            std::string authToken = GenerateAuthToken();
            int currentAuthTokenId = AddAuthToken(authToken, context);
            std::cout << "new token id: " << currentAuthTokenId << " UserId: " << userId << std::endl;
            boost::scoped_ptr<sql::PreparedStatement> updateTokenStatment(connection->prepareStatement("UPDATE `users` SET `token_id`=? WHERE `id` = ?"));
            updateTokenStatment->setInt(1, currentAuthTokenId);
            updateTokenStatment->setInt(2, userId);
            updateTokenStatment->executeUpdate();
            return authToken;
        }
    }
    return "";
}

bool NewUsersRepository::CheckAlive(const std::string authToken, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
        context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> findTokenStatement(connection->prepareStatement("SELECT COUNT(*), at.`id` FROM auth_tokens as at WHERE at.`token` = ? AND at.`is_enabled` = TRUE AND TIMESTAMPDIFF( SECOND , at.`last_used` , CURRENT_TIMESTAMP ) < ?"));
    int index = 1;
    findTokenStatement->setString(index++, authToken);
    findTokenStatement->setInt(index++, m_sessionExpartionTime);
    boost::scoped_ptr<sql::ResultSet> findTokenResult(findTokenStatement->executeQuery());
    while (findTokenResult->next())
    {
        bool res = findTokenResult->getInt(1) > 0;
        if (res)
        {
            boost::scoped_ptr<sql::PreparedStatement> updateTokenStatment(connection->prepareStatement("UPDATE  `auth_tokens` SET  `last_used` = CURRENT_TIMESTAMP WHERE  `id` = ?"));
            updateTokenStatment->setInt(1, findTokenResult->getInt(2));
            updateTokenStatment->executeUpdate();
        }
        return res;
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
    boost::scoped_ptr<sql::PreparedStatement> inserStatment(connection->prepareStatement("INSERT INTO `auth_tokens`(`token`, `is_enabled` ) VALUES( ? , 1 )"));
    inserStatment->setString(1, token);
    inserStatment->execute();
    connection->commit();
    boost::scoped_ptr<sql::PreparedStatement> getLastId(connection->prepareStatement("SELECT LAST_INSERT_ID() FROM `auth_tokens`"));
    boost::scoped_ptr<sql::ResultSet> result(getLastId->executeQuery());
    int insertId = -1;
    while (result->next())
    {
        insertId = result->getInt(1);
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
    deactivateStatment->setInt(1, id);
    int affectedCount = deactivateStatment->executeUpdate();
    if (affectedCount == 0)
    {
        std::cout << "No auth token found for id " << id;
    }
}

std::string NewUsersRepository::GenerateAuthToken()
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::string str = std::to_string(millis);
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

bool NewUsersRepository::IsMailUnique(const std::string& mail, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
        context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> countMailStatment(connection->prepareStatement("SELECT COUNT(*) FROM `users` where mail = ?"));
    countMailStatment->setString(1, mail);
    boost::scoped_ptr<sql::ResultSet> result(countMailStatment->executeQuery());
    while (result->next())
    {
        return result->getInt(1) == 0;
    }

    return true;

}
