/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mailer.h
 * Author: Ruslan
 *
 * Created on February 15, 2016, 11:16 PM
 */

#ifndef MAILER_H
#define MAILER_H
#include <fastcgi2/component.h>
class Mailer : public fastcgi::Component {
public:
    Mailer(fastcgi::ComponentContext* context);
    virtual void onLoad();
    virtual void onUnload();
    virtual ~Mailer();
private:

};

#endif /* MAILER_H */

