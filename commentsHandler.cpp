#include "commentsHandler.h"
#include "Subrouter.h"
#include "dao/CommentsRepository.h"
#include "JsonUtils.h"
#include "FcgiHelper.h"

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
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
using namespace boost::filesystem;
#define PAGE_SIZE 30

CommentsHandler::CommentsHandler(fastcgi::ComponentContext *context)
: fastcgi::Component(context)
, commentsCache(1500)
, m_pCommentsRepository(NULL)
, m_pRouter(new Subrouter)
{
    HandlerDescriptor* addCommentsHandler = m_pRouter->RegisterHandler(boost::bind(&CommentsHandler::handleAddCommentRequest, this, _1, _2));
    addCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/comments/(?<infohash>[a-fA-F0-9]{40})")));
    addCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("post")));
    HandlerDescriptor* getCommentsHandler = m_pRouter->RegisterHandler(boost::bind(&CommentsHandler::handleGetCommentsRequest, this, _1, _2));
    getCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new UrlFilter("/v1/comments/(?<infohash>[a-fA-F0-9]{40})(/page/(?<page>\\d+))?")));
    getCommentsHandler->Filters.push_back(boost::shared_ptr<RequestFilter>(new RequestTypeFilter("get")));
    std::cout << "CommentsHandler::ctor" << std::endl;


}

void CommentsHandler::onLoad()
{
    std::cout << "CommentsHandler::onLoad" << std::endl;
    mysql_host = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlhost");
    mysql_user = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqluser");
    mysql_pass = context()->getConfig()->asString(context()->getComponentXPath() + "/mysqlpass");
    m_pCommentsRepository.reset(new CommentsRepository(mysql_host, mysql_user, mysql_pass));
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
    boost::any infohashValue = handlerContext->getParam("infohash");
    if (!infohashValue.empty())
    {
        std::string infoHash = boost::any_cast<std::string>(infohashValue);
        boost::any pageValue = handlerContext->getParam("page");
        int page = 0;
        if (!pageValue.empty())
        {
            page = boost::lexical_cast<int>(boost::any_cast<std::string>(pageValue));
        }
        std::string cacheKey = infoHash;
        cacheKey.append(std::to_string(page));
        boost::scoped_ptr<rapidjson::Document> root(new rapidjson::Document(rapidjson::kObjectType));
        std::string scriptFileName = request->getScriptFilename();
        std::string remoteAdr = request->getRemoteAddr();


        std::vector<Comment> comments = m_pCommentsRepository->GetComments(infoHash, page);


        float avgRating = m_pCommentsRepository->GetCommentsAvarageRating(infoHash);
        int pageCount = m_pCommentsRepository->GetCommentsPageCount(infoHash);
        root->SetObject();
        root->AddMember("pageCount", pageCount, root->GetAllocator());
        root->AddMember("rating", avgRating, root->GetAllocator());
        std::cout << "Comments Count " << comments.size() << std::endl;
        buildJson(root.get(), comments);

        //   commentsCache.insert(cacheKey, root);

        FcgiHelper::WriteJson(request, *root);
        
        
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


        if (doc.IsObject() && doc.HasMember("comment"))
        {
            rapidjson::Value::ConstMemberIterator commentJsonObjectIter = doc.FindMember("comment");
            Comment comment;
            const rapidjson::Value& commentJsonObject = commentJsonObjectIter->value;
            comment.parentCommentId = JsonUtils::GetValue<int>(commentJsonObject, "parentId");
            comment.comment = JsonUtils::GetValue<std::string>(commentJsonObject, "data");
            comment.infohash = infoHash;
            comment.userToken = JsonUtils::GetValue<std::string>(commentJsonObject, "userToken");
            comment.rating = JsonUtils::GetValue<double>(commentJsonObject, "rating");
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
            std::string str(buffer);
            comment.comentTime = buffer;

            boost::unique_lock<boost::mutex> lock(queueMutex_);
            queue_.push_back(comment);
            queueCondition_.notify_one();
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
                comment.AddMember("comment_date", rapidjson::Value(commentObj.comentTime.c_str(), allocator), allocator);
                comment.AddMember("rating", commentObj.rating, allocator);
                AddChildComments(&comment, &commentObjs, &commentObj.childComments, allocator);
                comments.PushBack(comment, allocator);
            


        }
    }
    catch (std::exception ex)
    {
        std::cout << "Exception occured buildJson " << ex.what() << std::endl;
    }
    pt->AddMember("comments", comments, allocator);
}

void CommentsHandler::AddChildComments(rapidjson::Value* root, std::vector<Comment>* commentObjs, std::vector<int>* childComments, rapidjson::Document::AllocatorType& allocator)
{
    std::vector<Comment> childCommentObjs;
    std::vector<Comment>::iterator commentsBegin = commentObjs->begin(), commentsEnd = commentObjs->end();
    int childCommentsCount = childComments->size();

    for (int i = 0; i < childCommentsCount; i++)
    {
        int commentId = childComments->at(i);
        std::vector<Comment>::iterator found = std::find_if(commentsBegin, commentsEnd, [commentId](const Comment & comment)
        {
            return comment.id == commentId;
        });
        if (found != commentsEnd)
        {
            childCommentObjs.push_back(*found);
        }
    }

    std::vector<Comment>::iterator i = childCommentObjs.begin();
    rapidjson::Value comments(rapidjson::kArrayType);
    for (i; i != childCommentObjs.end(); i++)
    {
        Comment commentObj = *i;

        rapidjson::Value comment(rapidjson::kObjectType);
        comment.AddMember("id", commentObj.id, allocator);
        comment.AddMember("parent_comment_id", commentObj.parentCommentId, allocator);
        comment.AddMember("infohash", rapidjson::Value(commentObj.infohash.c_str(), allocator), allocator);
        comment.AddMember("data", rapidjson::Value(commentObj.comment.c_str(), allocator), allocator);
        comment.AddMember("user_id", rapidjson::Value(commentObj.userToken.c_str(), allocator), allocator);
        comment.AddMember("comment_date", rapidjson::Value(commentObj.comentTime.c_str(), allocator), allocator);
        comment.AddMember("rating", commentObj.rating, allocator);
        AddChildComments(&comment, commentObjs, &commentObj.childComments, allocator);
        comments.PushBack(comment, allocator);



    }
    root->AddMember("comments", comments, allocator);



}

void CommentsHandler::QueueProcessingThread()
{
    while (!stopping_)
    {
        std::vector<Comment> queueCopy;
        if (queueCopy.empty())
        {
            std::cout << "Before lock QueueProcessingThread" << std::endl;
            boost::unique_lock<boost::mutex> lock(queueMutex_);
            std::cout << "Waiting for new items..." << std::endl;
            queueCondition_.wait(lock);
            std::swap(queueCopy, queue_);
            if (queueCopy.empty())
                continue;
            std::cout << "After lock QueueProcessingThread" << std::endl;
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
