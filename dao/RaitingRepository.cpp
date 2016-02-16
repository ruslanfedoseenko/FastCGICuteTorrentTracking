/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RaitingRepository.cpp
 * Author: Ruslan
 * 
 * Created on February 8, 2016, 11:18 PM
 */

#include "RaitingRepository.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/driver.h>
#include <fastcgi2/config.h>
RaitingRepository::RaitingRepository(fastcgi::ComponentContext* componentContext) 
: BaseRepository()
, fastcgi::Component(componentContext)
{
    std::string rootXPath = context()->getComponentXPath();
    setDbHost(context()->getConfig()->asString(rootXPath + "/mysqlhost"));
    setDbName(context()->getConfig()->asString(rootXPath + "/mysqldbname"));
    setDbUser(context()->getConfig()->asString(rootXPath + "/mysqluser"));
    setDbPassword(context()->getConfig()->asString(rootXPath + "/mysqlpass"));
}

void RaitingRepository::AddRatings(const std::vector<Rating>& ratings, boost::shared_ptr<RepositoryContext> context)
{
    if (context == nullptr)
    {
	context = createContext();
    }
    boost::shared_ptr<sql::Connection> con = context->GetConnection();

    std::string query = "INSERT INTO `rating`(`uid`, `speed_mark`, `design_mark`, `possibilities_mark`, `usability_mark`, `custom_msg`) VALUES ";
    try
    {
	for (int i = 0; i < ratings.size(); i++)
	{
	    query.append("(?, ?, ?, ?, ?, ?) ,");
	}
	query.erase(query.length() - 1);
	int index = 1;
	boost::scoped_ptr<sql::PreparedStatement> statment(con->prepareStatement(query));
	for (std::vector<Rating>::const_iterator i = ratings.begin(); i != ratings.end(); ++i)
	{
	    Rating mark = *i;
	    statment->setString(index++, mark.uid);
	    statment->setDouble(index++, mark.speed);
	    statment->setDouble(index++, mark.design);
	    statment->setDouble(index++, mark.possibilities);
	    statment->setDouble(index++, mark.usability);
	    statment->setString(index++, mark.message);

	}
	statment->execute();

    }
    catch (sql::SQLException ex)
    {
	std::cout << "sql::SQLException occured:" << ex.what() << std::endl;
    }
}

