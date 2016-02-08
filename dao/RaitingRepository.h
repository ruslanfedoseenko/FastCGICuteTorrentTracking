/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RaitingRepository.h
 * Author: Ruslan
 *
 * Created on February 8, 2016, 11:18 PM
 */

#ifndef RAITINGREPOSITORY_H
#define RAITINGREPOSITORY_H

#include "BaseRepository.h"
#include "Rating.h"

class RaitingRepository : public BaseRepository {
public:
    RaitingRepository(const std::string& dbHost, const std::string& dbUser, const std::string& dbPassword);
    void AddRatings(const std::vector<Rating>& ratings, boost::shared_ptr<RepositoryContext> context = nullptr);
private:

};

#endif /* RAITINGREPOSITORY_H */

