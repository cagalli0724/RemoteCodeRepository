#ifndef HIDEFUNCHANDLE_H
#define HIDEFUNCHANDLE_H

//////////////////////////////////////////////////////////////////////
// HideFuncHandle.h - demonstrate how to add div and span block to  //
//                     hiden and unhiden the code blocks in webpages//
//  ver 1.0                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines HideFuncHandle class. HideFuncHandle Class is to add div and span block to 
hiden and unhiden the code blocks in webpages. Each file is associated with one HideFuncHandle instance.
We get the lines which should add this function according to the brace character and pre-treatment characters. 


Public Interface:
=================

- std::string ModifyForHideFunc(int lineNum, std::string str, bool &ifModify);
	//modify the line content such as add div block, /div block and add span.

Build Process:
==============
Required files
- HideFuncHandle.cpp, HideFuncHandle.h

Build commands
- devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 04 April 2017
- first release
*/

#include <unordered_map>
#include <utility> 
namespace CodeAnalysis
{
	enum LineType { DIV_BUTTON, DIV_END,NONE,DEF_DIV,DEF_END }; //for enumerating different div labels

	class HideFuncHandle
	{
	public:	
		//modify the line content  such as add div block, /div block and add span.
		std::string ModifyForHideFunc(int lineNum, std::string str, bool &ifModify);
	private:
		//if the str is ifdef, ifndef and endif
		LineType JudgeType(std::string str, int lineNum);
		//modify the line content which should be added the button.
		std::string ModifyForDIVBn(int , std::string str, bool &,std::string);
	
	};
}

#endif

