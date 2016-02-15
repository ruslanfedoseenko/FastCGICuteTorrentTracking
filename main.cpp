#include "onlineHandler.h"
#include "ratingHandler.h"
#include "commentsHandler.h"
#include "userHandler.h"
#include "registerHandler.h"
#include "core/Components/Mailer.h"
FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("OnlineHandlerFactory", OnlineHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("RaitingHandlerFactory", RaitingHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("CommentsHandlerFactory", CommentsHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("UserHandlerFactory", UserHandler)
FCGIDAEMON_ADD_DEFAULT_FACTORY("MailerFactory", Mailer)
FCGIDAEMON_ADD_DEFAULT_FACTORY("RegisterHandlerFactory", RegisterHandler)
FCGIDAEMON_REGISTER_FACTORIES_END()