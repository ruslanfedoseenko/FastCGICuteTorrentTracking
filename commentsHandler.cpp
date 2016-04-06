#include "commentsHandler.h"
#include "Subrouter.h"
#include "dao/CommentsRepository.h"
#include "JsonUtils.h"
#include "FcgiHelper.h"
#include "NewUsersRepository.h"

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <cppconn/datatype.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "Errors.h"
using namespace boost::filesystem;
#define PAGE_SIZE 30

CommentsHandler::CommentsHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, m_pRouter(new Subrouter)
, m_pCommentsRepository(nullptr)
, m_pAuthRepo(nullptr)   
{
    HandlerDescriptor* addCommentsHandler = m_pRouter->RegisterHandler(boost::bind(&CommentsHandler::handleAddCommentRequest, this, _1, _2));
    addCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/comments/(?<infohash>[a-fA-F0-9]{40})/")));
    addCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* getCommentsHandler = m_pRouter->RegisterHandler(boost::bind(&CommentsHandler::handleGetCommentsRequest, this, _1, _2));
    getCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/comments/(?<infohash>[a-fA-F0-9]{40})/(page/(?<page>\\d+))?(\\?userToken=(?<user_id>[a-fA-F0-9]{64}))?")));
    getCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("get")));
    getCommentsHandler->Filters.push_back(boost::shared_ptr<ParamFilter>(new ParamFilter(false, "userToken", "(?<user_id>[a-fA-F0-9]{64})")));
    HandlerDescriptor* editCommentsHandler = m_pRouter->RegisterHandler(boost::bind(&CommentsHandler::handleEditCommentsRequest, this, _1, _2));
    editCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/comment/edit/(?<comment_id>\\d+)")));
    editCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* deleteCommentsHandler = m_pRouter->RegisterHandler(boost::bind(&CommentsHandler::handleDeleteCommentRequest, this, _1, _2));
    deleteCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/comment/delete/(?<comment_id>\\d+)")));
    deleteCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    std::cout << "CommentsHandler::ctor" << std::endl;


}

void CommentsHandler::onLoad()
{
    std::cout << "CommentsHandler::onLoad" << std::endl;
    std::string comentRepoComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/comments-repository");
    m_pCommentsRepository = context()->findComponent<CommentsRepository>(comentRepoComponentName);
    std::string authRepoComponentName = context()->getConfig()->asString(context()->getComponentXPath() + "/user-auth-repo");
    m_pAuthRepo = context()->findComponent<NewUsersRepository>(authRepoComponentName);
    writingThread_ = boost::thread(boost::bind(&CommentsHandler::QueueProcessingThread, this));
}

void CommentsHandler::onUnload()
{
    std::cout << "CommentsHandler::onUnload" << std::endl;
    stopping_ = true;
    queueCondition_.notify_one();
    writingThread_.join();
}

void CommentsHandler::handleGetCommentsRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    try {
    boost::any infohashValue = handlerContext->getParam("infohash");
    boost::any userTokenValue = handlerContext->getParam("user_id");
    if (!infohashValue.empty())
    {
	std::string infoHash = boost::any_cast<std::string>(infohashValue);
	boost::any pageValue = handlerContext->getParam("page");
	int page = 0;
	if (!pageValue.empty())
	{
	    std::string pageStr = boost::any_cast<std::string>(pageValue);
	    if (!pageStr.empty())
	    {
		std::cout << "Page: " << pageStr << std::endl;
		page = boost::lexical_cast<int>(pageStr) - 1;
	    }
	    
	}
	int pageCount = m_pCommentsRepository->GetCommentsPageCount(infoHash);
	
	if (page + 1 > pageCount || page < 0)
	{
	    FcgiHelper::WriteError(request, InvalidPageNumber, boost::str(boost::format("Invalid PageNumber specified: %1%. PageNumber should fit [1,%2%]") % (page + 1) % pageCount));
	    return;
	}
	
	std::string cacheKey = infoHash;
	cacheKey.append(std::to_string(page));
	boost::scoped_ptr<rapidjson::Document> root(new rapidjson::Document(rapidjson::kObjectType));
	std::string scriptFileName = request->getScriptFilename();
	std::string remoteAdr = request->getRemoteAddr();

	std::vector<Comment> comments;
	std::string userToken;
	if (!userTokenValue.empty())
	{
	    userToken  = boost::any_cast<std::string>(userTokenValue);
	    std::cout << "User Token " << userToken << std::endl;
	    
	}
	
	if (userToken.empty())
	{
	     comments = m_pCommentsRepository->GetComments(infoHash, page);
	}
	else
	{
	    comments = m_pCommentsRepository->GetComments(infoHash, userToken, page);
	}


	float avgRating = m_pCommentsRepository->GetCommentsAvarageRating(infoHash);
	
	root->SetObject();
	root->AddMember("pageCount", pageCount, root->GetAllocator());
	root->AddMember("rating", avgRating, root->GetAllocator());
	std::cout << "Comments Count " << comments.size() << std::endl;
	buildJson(root.get(), comments);

	//   commentsCache.insert(cacheKey, root);

	FcgiHelper::WriteJson(request, *root);


    }
     }
    catch (std::exception ex)
    {
	std::cout << "Exception occurred handleGetCommentsRequest " << ex.what() << std::endl;
    }

}

void CommentsHandler::handleAddCommentRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    boost::any infohashValue = handlerContext->getParam("infohash");
    if (!infohashValue.empty())
    {
	std::string infoHash = boost::any_cast<std::string>(infohashValue);
	fastcgi::DataBuffer buffer = request->requestBody();

	rapidjson::Document doc;

	if (!JsonUtils::ParseJson(doc, buffer))
	{
	    FcgiHelper::WriteParseError(request, doc.GetParseError());
	    return;
	}


	if (doc.IsObject() && doc.HasMember("comment") && doc.HasMember("user_token"))
	{
	    rapidjson::Value::ConstMemberIterator commentJsonObjectIter = doc.FindMember("comment");
	    Comment comment;
	    const rapidjson::Value& commentJsonObject = commentJsonObjectIter->value;
	    comment.comment = JsonUtils::GetValue<std::string>(commentJsonObject, "data");
	    comment.infohash = infoHash;
	    comment.userToken = JsonUtils::GetValue<std::string>(doc, "user_token");
	    comment.rating = JsonUtils::GetValue<double>(commentJsonObject, "rating");
	    time_t rawtime;
	    struct tm * timeinfo;
	    char buffer[80];
	    time(&rawtime);
	    timeinfo = localtime(&rawtime);

	    strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
	    std::string str(buffer);
	    comment.comentAddDateTime = buffer;
	    if (m_pAuthRepo->CheckAlive(comment.userToken))
	    {
		boost::unique_lock<boost::mutex> lock(queueMutex_);
		queue_.push_back(comment);
		queueCondition_.notify_one();
	    }
	    else
	    {
		FcgiHelper::WriteError(request, TokenExpired, "Token expired, authentification requred");
	    }
	    
	}
	else
	{
	    std::cout << "Required comment json object is missing." << std::endl;
	    request->setStatus(400);
	    return;
	}
    }
}

void CommentsHandler::handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *handlerContext)
{
    m_pRouter->HandleRequest(request, handlerContext);
}

void CommentsHandler::buildJson(rapidjson::Document* pt, std::vector<Comment>& commentObjs)
{
    std::vector<Comment>::iterator i = commentObjs.begin();
    rapidjson::Value comments(rapidjson::kArrayType);
    rapidjson::Document::AllocatorType& allocator = pt->GetAllocator();
    try
    {
	for (i; i != commentObjs.end(); i++)
	{
	    Comment commentObj = *i;

	    rapidjson::Value comment(rapidjson::kObjectType);
	    comment.AddMember("id", commentObj.id, allocator);
	    comment.AddMember("infohash", rapidjson::Value(commentObj.infohash.c_str(), allocator), allocator);
	    comment.AddMember("data", rapidjson::Value(commentObj.comment.c_str(), allocator), allocator);
	    comment.AddMember("user_id", rapidjson::Value(commentObj.userToken.c_str(), allocator), allocator);
	    comment.AddMember("comment_date", rapidjson::Value(commentObj.comentAddDateTime.c_str(), allocator), allocator);
	    comment.AddMember("rating", commentObj.rating, allocator);
	    comment.AddMember("can_edit", commentObj.canEdit, allocator);
	    comments.PushBack(comment, allocator);



	}
    }
    catch (std::exception ex)
    {
	std::cout << "Exception occured buildJson " << ex.what() << std::endl;
    }
    pt->AddMember("comments", comments, allocator);
}

void CommentsHandler::QueueProcessingThread()
{
    while (!stopping_)
    {
	std::vector<Comment> queueCopy;
	if (queueCopy.empty())
	{
	    std::cout << "Before lock CommentsHandler::QueueProcessingThread" << std::endl;
	    boost::unique_lock<boost::mutex> lock(queueMutex_);
	    std::cout << "Waiting for new items..." << std::endl;
	    queueCondition_.wait(lock);
	    std::swap(queueCopy, queue_);
	    if (queueCopy.empty())
		continue;
	    std::cout << "After lock CommentsHandler::QueueProcessingThread" << std::endl;
	}
	try
	{
	    m_pCommentsRepository->AddComments(queueCopy);
	    queueCopy.clear();
	}
	catch (sql::SQLException ex)
	{
	    std::cout << "sql::SQLException occured:" << ex.what() << std::endl;
	}
    }

}

void CommentsHandler::handleDeleteCommentRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext)
{
    boost::any idValue = handlerContext->getParam("comment_id");
    if (!idValue.empty())
    {
	std::string idString = boost::any_cast<std::string>(idValue);
	int comment_id = boost::lexical_cast<int>(idString);
	fastcgi::DataBuffer buffer = request->requestBody();

	rapidjson::Document doc;

	if (!JsonUtils::ParseJson(doc, buffer))
	{
	    FcgiHelper::WriteParseError(request, doc.GetParseError());
	    return;
	}


	if (doc.IsObject() && doc.HasMember("user_token"))
	{
	    rapidjson::Value::ConstMemberIterator commentJsonObjectIter = doc.FindMember("comment");
	    std::string userToken = JsonUtils::GetValue<std::string>(doc, "user_token");
	    std::cout << "Delete comment ID: " << comment_id << " UserToekn: " << userToken << std::endl;
	    if (m_pCommentsRepository->CheckCommentToken(userToken, comment_id))
	    {
		m_pCommentsRepository->DeleteComment(comment_id);
	    }
	    else
	    {
		FcgiHelper::WriteError(request, TokenExpired, "Token expired, authentification requred");
	    }
	    
	}
	else
	{
	    std::cout << "Required comment json object is missing." << std::endl;
	    request->setStatus(400);
	    return;
	}
    }
}


void CommentsHandler::handleEditCommentsRequest(fastcgi::Request* request, fastcgi::HandlerContext* handlerContext)
{
    boost::any idValue = handlerContext->getParam("comment_id");
    if (!idValue.empty())
    {
	std::string idString = boost::any_cast<std::string>(idValue);
	int comment_id = boost::lexical_cast<int>(idString);
	fastcgi::DataBuffer buffer = request->requestBody();

	rapidjson::Document doc;

	if (!JsonUtils::ParseJson(doc, buffer))
	{
	    FcgiHelper::WriteParseError(request, doc.GetParseError());
	    return;
	}


	if (doc.IsObject() && doc.HasMember("comment") && doc.HasMember("user_token"))
	{
	    rapidjson::Value::ConstMemberIterator commentJsonObjectIter = doc.FindMember("comment");
	    Comment comment;
	    const rapidjson::Value& commentJsonObject = commentJsonObjectIter->value;
	    comment.id = comment_id;
	    comment.comment = JsonUtils::GetValue<std::string>(commentJsonObject, "data");
	    comment.userToken = JsonUtils::GetValue<std::string>(doc, "user_token");
	    comment.rating = JsonUtils::GetValue<double>(commentJsonObject, "rating");
	    time_t rawtime;
	    struct tm * timeinfo;
	    char buffer[80];
	    time(&rawtime);
	    timeinfo = localtime(&rawtime);

	    strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
	    std::string str(buffer);
	    comment.comentUpdateDateTime = buffer;
	    if (m_pCommentsRepository->CheckCommentToken(comment.userToken, comment.id))
	    {
		boost::unique_lock<boost::mutex> lock(queueMutex_);
		queue_.push_back(comment);
		queueCondition_.notify_one();
	    }
	    else
	    {
		FcgiHelper::WriteError(request, TokenExpired, "Token expired, authentification requred");
	    }
	    
	}
	else
	{
	    std::cout << "Required comment json object is missing." << std::endl;
	    request->setStatus(400);
	    return;
	}
    }
}
