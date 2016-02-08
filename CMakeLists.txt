cmake_minimum_required(VERSION 3.3)
project(libTrackingFastCGI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    dao/BaseRepository.cpp
    dao/BaseRepository.h
    dao/CommentsRepository.cpp
    dao/CommentsRepository.h
    dao/RepositoryContext.cpp
    dao/RepositoryContext.h
    dao/UserRepository.cpp
    dao/UserRepository.h
    dao/UserRequest.h
    models/Comments.h
    models/Rating.h
    utils/TimeUtils.cpp
    utils/TimeUtils.h
    commentsHandler.cpp
    commentsHandler.h
    main.cpp
    onlineHandler.cpp
    onlineHandler.h
    ratingHandler.cpp
    ratingHandler.h
    registerHandler.cpp
    registerHandler.h
    userHandler.cpp
    userHandler.h)

add_executable(libTrackingFastCGI ${SOURCE_FILES})