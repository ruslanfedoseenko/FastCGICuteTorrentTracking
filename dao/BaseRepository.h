/*		
  * To change this license header, choose License Headers in Project Properties.		
  * To change this template file, choose Tools | Templates		
  * and open the template in the editor.		
  */		
 		
 /* 		
  * File:   BaseRepository.h		
  * Author: Ruslan		
  *		
  * Created on January 9, 2016, 1:12 PM		
  */		
 		
 #ifndef BASEREPOSITORY_H		
 #define BASEREPOSITORY_H		
 #include "RepositoryContext.h"		
 #include <boost/smart_ptr.hpp>		
 		
 class BaseRepository {		
 public:		
 		
     		
      BaseRepository() {};		
     virtual ~BaseRepository();		
 protected:		
     std::string getDbHost() const {		
         return m_dbHost;		
     }		
 		
     void setDbHost(std::string dbHost) {		
         m_dbHost = dbHost;		
     }		
 		
     std::string getDbName() const {		
         return m_dbName;		
     }		
 		
     void setDbName(std::string dbName) {		
         m_dbName = dbName;		
     }		
 		
     std::string getDbPassword() const {		
         return m_dbPassword;		
     }		
 		
     void setDbPassword(std::string dbPassword) {		
         m_dbPassword = dbPassword;		
     }		
 		
     std::string getDbUser() const {		
         return m_dbUser;		
     }		
 		
     void setDbUser(std::string dbUser) {		
         m_dbUser = dbUser;		
     }		
 		
 private:		
     std::string m_dbHost, m_dbUser, m_dbPassword, m_dbName;		
 protected:		
     boost::shared_ptr<RepositoryContext> createContext();		
 		
 };		
 		
 #endif /* BASEREPOSITORY_H */