//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Design Mode
//  @ File Name : Text.h
//  @ Date : 2012-3-16
//  @ Author : braveyly
//
//


#if !defined(_TEXT_H)
#define _TEXT_H

#include "HelpHandler.h"

class Text : public HelpHandler {
public:
	void print_helper(HelpType);
	Text(HelpType type, HelpHandler* handler):HelpHandler(type,handler){};
	~Text(){};
};

#endif  //_TEXT_H
