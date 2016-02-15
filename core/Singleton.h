/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Singleton.h
 * Author: Ruslan
 *
 * Created on February 15, 2016, 10:59 PM
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <boost/smart_ptr.hpp>


#define DEFINE_PTR_CALSS(T) class T;\
	typedef boost::shared_ptr<T> T##Ptr;

template<typename T>
class Singleton
{

public:
	static boost::shared_ptr<T> getInstance();
private:
	static boost::shared_ptr<T> m_pInstance;
};

template <typename T>
boost::shared_ptr<T>  Singleton<T>::m_pInstance;

template <typename T>
boost::shared_ptr<T> Singleton<T>::getInstance()
{
	

	if (!m_pInstance)
	{
		m_pInstance.reset(new T());
		
	}

	return m_pInstance;
}

#endif /* SINGLETON_H */

