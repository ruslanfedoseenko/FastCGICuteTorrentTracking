#include "onlineHandler.h"
#include "ratingHandler.h"
#include "commentsHandler.h"

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("OnlineHandlerFactory", OnlineHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("RaitingHandlerFactory", RaitingHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("CommentsHandlerFactory", CommentsHandler)
FCGIDAEMON_REGISTER_FACTORIES_END()