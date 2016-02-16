/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CommentsRepository.cpp
 * Author: Ruslan
 * 
 * Created on January 8, 2016, 4:42 PM
 */


#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>
#include <cppconn/driver.h>
#include <boost/algorithm/string.hpp>
#include <boost/thread/locks.hpp>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/smart_ptr.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include "CommentsRepository.h"
#include "NewUsersRepository.h"
#include <set>
#include <HashUtils.h>
#include <cppconn/datatype.h>
#include <fastcgi2/component.h>
#include <fastcgi2/config.h>
CommentsRepository::CommentsRepository(fastcgi::ComponentContext* componentContext)
: BaseRepository()
, fastcgi::Component(componentContext)
, m_pAuthRepo(nullptr)
, m_commentsCache(1500)
{
    std::string rootXPath = context()->getComponentXPath();
    setDbHost(context()->getConfig()->asString(rootXPath + "/mysqlhost"));
    setDbName(context()->getConfig()->asString(rootXPath + "/mysqldbname"));
    setDbUser(context()->getConfig()->asString(rootXPath + "/mysqluser"));
    setDbPassword(context()->getConfig()->asString(rootXPath + "/mysqlpass"));
    std::string userAuthRepoName = context()->getConfig()->asString(rootXPath + "/user-auth-repo");
    m_pAuthRepo.reset(context()->findComponent<NewUsersRepository>(userAuthRepoName));
}

void CommentsRepository::onLoad()
{
    
}

void CommentsRepository::onUnload()
{
    
}
std::vector<Comment> CommentsRepository::GetComments(std::string infoHash, int page, boost::shared_ptr<RepositoryContext> context)
{

    boost::mutex::scoped_lock lock(m_readMutex);
    std::cout << "CommentsRepository::GetComments " << infoHash << " " << page << std::endl;
    std::vector<Comment> comments;

    std::string cacheKey = infoHash;
    cacheKey.append(std::to_string(page));
    uint32_t crcCacheKey = HashUtils::CalculateCrc32(cacheKey);
    std::cout << "looking cache entry with cacheKey " << cacheKey << std::endl;
    if (m_commentsCache.count(crcCacheKey) > 0 && m_commentsCache.check(crcCacheKey))
    {
	std::cout << "entry with cacheKey " << cacheKey << " found" << std::endl;
	comments = m_commentsCache.fetch(crcCacheKey);
    }
    else
    {
	std::cout << "entry with cacheKey " << cacheKey << " not found" << std::endl;
	if (context == nullptr)
	{
	    context = createContext();
	}
	boost::shared_ptr<sql::Connection> connection = context->GetConnection();
	boost::scoped_ptr<sql::PreparedStatement> getCommentsStatment(connection->prepareStatement("SELECT tc1.`id`, tc1.`infohash`, tc1.`comment_data`, u.`username`, tc1.`added_date`, tc1.`updated_date`, tc1.`rating` FROM `torrent_coments` as tc1 LEFT JOIN `users` u ON u.id = tc1.`user_id` WHERE `infohash`=? ORDER BY tc1.`added_date` LIMIT ?, ?"));
	getCommentsStatment->setString(1, infoHash);
	getCommentsStatment->setInt(2, page * PAGE_SIZE);
	getCommentsStatment->setInt(3, PAGE_SIZE);
	boost::scoped_ptr<sql::ResultSet> commentsResultSet(getCommentsStatment->executeQuery());
	while (commentsResultSet->next())
	{
	    Comment comment;
	    comment.id = commentsResultSet->getInt("id");
	    comment.infohash = commentsResultSet->getString("infohash");
	    comment.comment = commentsResultSet->getString("comment_data");
	    comment.userToken = commentsResultSet->getString("username");
	    comment.comentUpdateDateTime = commentsResultSet->getString("updated_date");
	    comment.comentAddDateTime = commentsResultSet->getString("added_date");
	    comment.rating = commentsResultSet->getDouble("rating");
	    comments.push_back(comment);
	}
	if (comments.size() > 0)
	{
	    std::cout << "inserting cache entry with cacheKey " << cacheKey << std::endl;
	    m_commentsCache.insert(crcCacheKey, comments);
	}
    }



    return comments;
}

bool CommentsRepository::CheckCommentToken(std::string token, int comment_id, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    bool isTokenAlive = m_pAuthRepo->CheckAlive(token, context);
    if (!isTokenAlive)
	return isTokenAlive;
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> isValidTokenForIdStatement(connection->prepareStatement("SELECT (SELECT u.`id` FROM  `users` AS u LEFT JOIN auth_tokens AS at ON u.`token_id` = at.`id` WHERE at.`token` =  ?  LIMIT 1 ) = ( SELECT  `user_id` FROM  `torrent_coments` WHERE  `id` =? LIMIT 1)"));
    isValidTokenForIdStatement->setString(1, token);
    isValidTokenForIdStatement->setInt(2, comment_id);
    boost::scoped_ptr<sql::ResultSet> isValidTokenForIdResultSet(isValidTokenForIdStatement->executeQuery());
    bool valid;
    while (isValidTokenForIdResultSet->next())
    {
	valid = isValidTokenForIdResultSet->getInt(1) > 0;
    }

    return valid;
}

int CommentsRepository::GetCommentsPageCount(std::string infoHash, boost::shared_ptr<RepositoryContext> context)
{
    boost::mutex::scoped_lock lock(m_readMutex);
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> pageCountStatement(connection->prepareStatement("SELECT ROUND(COUNT(*)/(?) + 0.5) AS pageCount FROM `torrent_coments` WHERE `infohash`=(?)"));
    pageCountStatement->setInt(1, PAGE_SIZE);
    pageCountStatement->setString(2, infoHash);
    boost::scoped_ptr<sql::ResultSet> pageCountResultSet(pageCountStatement->executeQuery());
    int pageCount = 1;
    while (pageCountResultSet->next())
    {
	pageCount = pageCountResultSet->getInt(1);
    }

    return pageCount;
}

float CommentsRepository::GetCommentsAvarageRating(std::string infoHash, boost::shared_ptr<RepositoryContext> context)
{
    boost::mutex::scoped_lock lock(m_readMutex);
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> connection = context->GetConnection();
    boost::scoped_ptr<sql::PreparedStatement> avgRatingStatement(connection->prepareStatement("SELECT ((SUM(  `rating` ) + 31.25 /5 * MAX(  `rating` )) / ( COUNT(  `rating` ) +10 )) AS ci_lower_bound FROM  `torrent_coments` WHERE `infohash`=(?)"));
    avgRatingStatement->setString(1, infoHash);
    boost::scoped_ptr<sql::ResultSet> ratingResultSet(avgRatingStatement->executeQuery());
    float avgRating = -1.f;
    while (ratingResultSet->next())
    {
	avgRating = ratingResultSet->getDouble(1);
    }

    return avgRating;
}

void CommentsRepository::AddComments(const std::vector<Comment>& comments, boost::shared_ptr<RepositoryContext> context)
{
    std::set<std::string> cachesToInvalidate;
    {
	boost::mutex::scoped_lock lock(m_readMutex);
	if (context == nullptr)
	{
	    context = createContext();
	}

	boost::shared_ptr<sql::Connection> connection = context->GetConnection();
	std::string insertStatementSql = "INSERT INTO `torrent_coments` (`id`, `infohash`, `comment_data`, `user_id`, `added_date`, `updated_date` , `rating`) VALUES ";
	for (int i=0; i < comments.size(); i++)
	{
	    insertStatementSql.append("(?, ?, ?, ?, ?, ?, ?) ,");
	}
	insertStatementSql.erase(insertStatementSql.length() - 1);
	insertStatementSql.append(" ON DUPLICATE KEY UPDATE `comment_data` = VALUES(`comment_data`), updated_date = CURRENT_TIMESTAMP, `updated_date`=VALUES(`updated_date`), `rating`= VALUES(`rating`) ");
	boost::scoped_ptr<sql::PreparedStatement> statment(connection->prepareStatement(insertStatementSql));
	int index = 1;
	
	for (std::vector<Comment>::const_iterator i = comments.begin(); i != comments.end(); ++i)
	{
	    Comment comment = *i;
	    int userId = m_pAuthRepo->GetUserIdByActiveToken(comment.userToken, context);
	    if (userId > 0)
	    {
		if (comment.id == 0)
		{
		    statment->setNull(index++, sql::DataType::INTEGER);
		}
		else
		{
		    std::cout << "setting id in query " << comment.id << std::endl;
		    statment->setInt(index++, comment.id);
		}
		cachesToInvalidate.insert(comment.infohash);
		statment->setString(index++, comment.infohash);
		statment->setString(index++, comment.comment);
		statment->setInt(index++, userId);
		statment->setDateTime(index++, comment.comentAddDateTime);
		if (comment.comentUpdateDateTime.empty())
		    statment->setNull(index++, sql::DataType::TIMESTAMP);
		else
		    statment->setDateTime(index++, comment.comentUpdateDateTime);
		statment->setDouble(index++, comment.rating);
	    }
	    else
	    {
		std::cout << "No user found for token " << comment.userToken;
	    }

	}
	statment->execute();
    }
    for (std::set<std::string>::iterator i = cachesToInvalidate.begin(); i != cachesToInvalidate.end(); ++i)
    {
	std::string infoHash = *i;
	int pages = GetCommentsPageCount(infoHash, context);
	for (int j = 0; j < pages; j++)
	{
	    std::string cacheKey = infoHash + std::to_string(j);
	    uint32_t crcCacheKey = HashUtils::CalculateCrc32(cacheKey);
	    if (m_commentsCache.check(crcCacheKey))
		m_commentsCache.erase(crcCacheKey);
	}

    }

}

CommentsRepository::~CommentsRepository()
{
}

