//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Design Mode Mediator
//  @ File Name : ColleageA.cpp
//  @ Date : 2012-3-19
//  @ Author : braveyly
//
//


#include "ColleageA.h"

ColleageA::ColleageA(Mediator* mdt):Colleage(mdt)
{}

void ColleageA::action() {
	get_mediator()->doActoinFromBToA();
	cout<<"a state is"<<get_state()<<endl;

}

