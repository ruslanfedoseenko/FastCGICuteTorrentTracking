#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/commentsHandler.o \
	${OBJECTDIR}/core/Components/Mailer.o \
	${OBJECTDIR}/dao/BaseRepository.o \
	${OBJECTDIR}/dao/CommentsRepository.o \
	${OBJECTDIR}/dao/JsonUtils.o \
	${OBJECTDIR}/dao/NewUsersRepository.o \
	${OBJECTDIR}/dao/RaitingRepository.o \
	${OBJECTDIR}/dao/RepositoryContext.o \
	${OBJECTDIR}/dao/UserRepository.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/onlineHandler.o \
	${OBJECTDIR}/ratingHandler.o \
	${OBJECTDIR}/registerHandler.o \
	${OBJECTDIR}/userHandler.o \
	${OBJECTDIR}/utils/FcgiHelper.o \
	${OBJECTDIR}/utils/HashUtils.o \
	${OBJECTDIR}/utils/Subrouter.o \
	${OBJECTDIR}/utils/TimeUtils.o \
	${OBJECTDIR}/utils/fastcgi2/RequestFilters.o \
	${OBJECTDIR}/utils/fastcgi2/VariableExtractor.o \
	${OBJECTDIR}/utils/mail/CSmtp.o \
	${OBJECTDIR}/utils/mail/base64.o \
	${OBJECTDIR}/utils/mail/main.o \
	${OBJECTDIR}/utils/mail/md5.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-O2
CXXFLAGS=-O2

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lfastcgi-daemon2 -lmysqlcppconn -lmysqlcppconn-static -lboost_system -lboost_filesystem -lfastcgi-daemon2 -lmysqlcppconn -lmysqlcppconn-static -lboost_system -lboost_filesystem -lssl -lcrypto

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libTrackingFastCGI.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libTrackingFastCGI.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libTrackingFastCGI.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/commentsHandler.o: commentsHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/commentsHandler.o commentsHandler.cpp

${OBJECTDIR}/core/Components/Mailer.o: core/Components/Mailer.cpp 
	${MKDIR} -p ${OBJECTDIR}/core/Components
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/Components/Mailer.o core/Components/Mailer.cpp

${OBJECTDIR}/dao/BaseRepository.o: dao/BaseRepository.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/BaseRepository.o dao/BaseRepository.cpp

${OBJECTDIR}/dao/CommentsRepository.o: dao/CommentsRepository.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/CommentsRepository.o dao/CommentsRepository.cpp

${OBJECTDIR}/dao/JsonUtils.o: dao/JsonUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/JsonUtils.o dao/JsonUtils.cpp

${OBJECTDIR}/dao/NewUsersRepository.o: dao/NewUsersRepository.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/NewUsersRepository.o dao/NewUsersRepository.cpp

${OBJECTDIR}/dao/RaitingRepository.o: dao/RaitingRepository.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/RaitingRepository.o dao/RaitingRepository.cpp

${OBJECTDIR}/dao/RepositoryContext.o: dao/RepositoryContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/RepositoryContext.o dao/RepositoryContext.cpp

${OBJECTDIR}/dao/UserRepository.o: dao/UserRepository.cpp 
	${MKDIR} -p ${OBJECTDIR}/dao
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dao/UserRepository.o dao/UserRepository.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/onlineHandler.o: onlineHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/onlineHandler.o onlineHandler.cpp

${OBJECTDIR}/ratingHandler.o: ratingHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ratingHandler.o ratingHandler.cpp

${OBJECTDIR}/registerHandler.o: registerHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/registerHandler.o registerHandler.cpp

${OBJECTDIR}/userHandler.o: userHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/userHandler.o userHandler.cpp

${OBJECTDIR}/utils/FcgiHelper.o: utils/FcgiHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/FcgiHelper.o utils/FcgiHelper.cpp

${OBJECTDIR}/utils/HashUtils.o: utils/HashUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/HashUtils.o utils/HashUtils.cpp

${OBJECTDIR}/utils/Subrouter.o: utils/Subrouter.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/Subrouter.o utils/Subrouter.cpp

${OBJECTDIR}/utils/TimeUtils.o: utils/TimeUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/TimeUtils.o utils/TimeUtils.cpp

${OBJECTDIR}/utils/fastcgi2/RequestFilters.o: utils/fastcgi2/RequestFilters.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils/fastcgi2
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/fastcgi2/RequestFilters.o utils/fastcgi2/RequestFilters.cpp

${OBJECTDIR}/utils/fastcgi2/VariableExtractor.o: utils/fastcgi2/VariableExtractor.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils/fastcgi2
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/fastcgi2/VariableExtractor.o utils/fastcgi2/VariableExtractor.cpp

${OBJECTDIR}/utils/mail/CSmtp.o: utils/mail/CSmtp.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/mail/CSmtp.o utils/mail/CSmtp.cpp

${OBJECTDIR}/utils/mail/base64.o: utils/mail/base64.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/mail/base64.o utils/mail/base64.cpp

${OBJECTDIR}/utils/mail/main.o: utils/mail/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/mail/main.o utils/mail/main.cpp

${OBJECTDIR}/utils/mail/md5.o: utils/mail/md5.cpp 
	${MKDIR} -p ${OBJECTDIR}/utils/mail
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Idao -Imodels -Iutils -Iutils/fastcgi2 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/mail/md5.o utils/mail/md5.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libTrackingFastCGI.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
