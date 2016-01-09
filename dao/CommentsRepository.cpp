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
#include <set>

CommentsRepository::CommentsRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword)
: BaseRepository(dbHost, dbUser, dbPassword)
, m_commentsCache(1500)
{

}

std::vector<Comment> CommentsRepository::GetComments(std::string infoHash, int page, boost::shared_ptr<RepositoryContext> context)
{
    std::cout << "CommentsRepository::GetComments " << infoHash << " " << page << std::endl;
    std::vector<Comment> comments;
    std::string cacheKey = infoHash;
    cacheKey.append(std::to_string(page));
    if (m_commentsCache.check(cacheKey))
    {
	comments = m_commentsCache.fetch(cacheKey);
    }
    else
    {
	boost::mutex::scoped_lock lock(m_readMutex);
	if (context == nullptr)
	{
	    context = createContext();
	}
	boost::shared_ptr<sql::Connection> connection = context->GetConnection();
	boost::scoped_ptr<sql::PreparedStatement> getCommentsStatment(connection->prepareStatement("SELECT tc1.`id`, tc1.`parent_comment_id`, tc1.`infohash`, tc1.`comment`, tc1.`uid`, tc1.`comment_date`, tc1.`rating`, (SELECT  GROUP_CONCAT(tc2.`id` SEPARATOR ',')  FROM `torrent_coments` as tc2 where tc2.`parent_comment_id` = tc1.`id`) AS ChildrenComments  FROM `torrent_coments` as tc1 WHERE `infohash`=? ORDER BY tc1.`comment_date` LIMIT ?, ?"));
	getCommentsStatment->setString(1, infoHash);
	getCommentsStatment->setInt(2, page * PAGE_SIZE);
	getCommentsStatment->setInt(3, (page + 1) * PAGE_SIZE);
	boost::scoped_ptr<sql::ResultSet> commentsResultSet(getCommentsStatment->executeQuery());
	while (commentsResultSet->next())
	{
	    Comment comment;
	    comment.id = commentsResultSet->getInt("id");
	    comment.parentCommentId = commentsResultSet->isNull("parent_comment_id") ? -1 : commentsResultSet->getInt("parent_comment_id");
	    comment.infohash = commentsResultSet->getString("infohash");
	    comment.comment = commentsResultSet->getString("comment");
	    comment.userToken = commentsResultSet->getString("uid");
	    comment.comentTime = commentsResultSet->getString("comment_date");
	    comment.rating = commentsResultSet->getDouble("rating");
	    if (!commentsResultSet->isNull("ChildrenComments"))
	    {
		std::string childrenComments = commentsResultSet->getString("ChildrenComments");
		std::vector<std::string> strs;

		if (!childrenComments.empty())
		{
		    boost::split(strs, childrenComments, boost::is_any_of(","));
		    for (int i = 0; i < strs.size(); i++)
		    {
			comment.childComments.push_back(std::stoi(strs[i]));
		    }
		}
	    }
	    comments.push_back(comment);
	}
	if (comments.size() > 0)
	    m_commentsCache.insert(cacheKey, comments);
    }



    return comments;
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
	std::string insertStatementSql = "INSERT INTO `torrent_coments` (`parent_comment_id`, `infohash`, `comment`, `uid`, `comment_date`, `rating`) VALUES ";
	for (int i=0; i < comments.size(); i++)
	{
	    insertStatementSql.append("(?, ?, ?, ?, ?, ?) ,");
	}
	insertStatementSql.erase(insertStatementSql.length() - 1);
	boost::scoped_ptr<sql::PreparedStatement> statment(connection->prepareStatement(insertStatementSql));
	int index = 1;
	for (std::vector<Comment>::const_iterator i = comments.begin(); i != comments.end(); ++i)
	{
	    Comment comment = *i;
	    if (comment.parentCommentId <= 0)
	    {
		statment->setNull(index++, sql::DataType::INTEGER);
	    }
	    else
	    {
		statment->setInt(index++, comment.parentCommentId);
	    }
	    cachesToInvalidate.insert(comment.infohash);
	    statment->setString(index++, comment.infohash);
	    statment->setString(index++, comment.comment);
	    statment->setString(index++, comment.userToken);
	    statment->setDateTime(index++, comment.comentTime);
	    statment->setDouble(index++, comment.rating);
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
	    if (m_commentsCache.check(cacheKey))
		m_commentsCache.erase(cacheKey);
	}

    }

}

CommentsRepository::~CommentsRepository()
{
}

