/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VariableExtractor.h
 * Author: RuslanF
 *
 * Created on February 10, 2016, 4:45 PM
 */

#ifndef VARIABLEEXTRACTOR_H
#define VARIABLEEXTRACTOR_H

class VariableExtractor {
public:
    template <class TVar>
    TVar Extract(boost::smatch match, const std::string& vraibleName);
    virtual ~VariableExtractor();
private:

};



#endif /* VARIABLEEXTRACTOR_H */

