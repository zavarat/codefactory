//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Disign Mode No-Mediator
//  @ File Name : Colleage.h
//  @ Date : 2012-3-19
//  @ Author : braveyly
//
//


#if !defined(_COLLEAGE_H)
#define _COLLEAGE_H
#include <iostream>
#include <string>
using namespace::std;

class Colleage {
public:
	string get_state();
	void set_state(string);
	Colleage* get_related_colleage();
	void set_colleage(Colleage*);
	virtual void action();
	Colleage(Colleage*);
	Colleage();
private:
	string state;
	Colleage* _related_colleage;
};

#endif  //_COLLEAGE_H
