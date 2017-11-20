

#ifndef DEPANAL_H
#define DEPANAL_H
//////////////////////////////////////////////////////////////////////
//  DepAnal.h - demonstrate how to analyze the dependency           //
//  ver 1.1                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines DepAnal class. DepAnal Class is to analyze the depedencies of a file on files 
based on the analysis of "#include" sentences in files..
The dependencies will be stored in NoSqlDb<string> Class. The key of NoSqlDb is the file fullname;
children in the element in NoSqlDb is the file name which the key file dependencies on.
DepAnal will use DepDisplay Class to show the result of dependency analysis.


Public Interface:
=================

- DepAnal();				//constructor
- void getDepfileList		//get the file list that dependent on the specified file name
- void getFileName          //get the list of file names which we should analyze
- deleteFile                //delete file
- SCAnal                    //analysis of strong component
- DependencyAnalysis        //do the actual action of dependency Analysis
- depDisplay                //show the dependency Analysis
- saveToFile                //save to File

Build Process:
==============
Required files
- DepAnal.h, DepAnal.cpp, DepDisplay.h, DepDisplay.cpp, Executive.h, Executive.cpp
- FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp, tokenizer.h, tokenizer.cpp
- IFileMgr.h, IFileMgr.cpp, Logger.h, Logger.cpp, TypeTable.h, TypeTable.cpp, 
- Utilities.h, Utilities.cpp, XmlWriter.h, XmlWriter.cpp,

Build commands 
- devenv Project3_0331.sln /rebuild debug

Maintenance History:
====================
ver 1.1 : 02 May 2017
- remove the analysis of code analysis and type table to simplize the code
ver 1.0 : 07 Mar 2017
- first release
*/

#include "../FileMgr/FileMgr.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../StrongComp/StrongComp.h"
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#pragma warning (disable : 4101)  // disable warning re unused variable x, below





namespace CodeAnalysis
{
	class DepAnal
	{
	public:
		using Key = std::string;
		using Keys = std::vector<Key>;
		using File = std::string;
		using SElemPtr = std::shared_ptr<NoSQLDB::Element<std::string>>;
		//constructor
		DepAnal();
		//get the file list that dependent on the specified file name
		void getDepfileList(std::string fileName, std::vector<std::string>& depfileList);
		//get FileList by categoryName
		std::vector<std::string> getFileListbyCategory(std::string categoryName);
		//delete file
		void deleteFile(std::string path);
		//analysis of strong component 
		void SCAnal(std::string );
		//do the actual action of dependency Analysis 
		void DependencyAnalysis(std::string,std::string);
		//show the dependency Analysis
		void depDisplay(int size);
		//save to File
		void saveToFile(std::string);
	private:
		//do dependency anlaysis for each file.
		void DepAnalForFile(std::ifstream &, NoSQLDB::Element<std::string>&, std::string);
		
		//using strongComp Class to analysis the strong component
		StrongComp sc;   
		// store the result of dependency analysis.
		NoSQLDB::NoSqlDb<std::string> myDb; 
		
		
	};

	
}

#endif 