
#ifndef CODEPUBLISHER_H
#define CODEPUBLISHER_H

//////////////////////////////////////////////////////////////////////
//  CodePublisher.h - demonstrate how to publish the code pages     //
//  ver 1.1                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr3, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines CodePublisher class. CodePublisher Class is to publish the code pages in the specified
directory. The code pages was changed into html webpages by doPublish() function. 
First, get the directory for publishing by getDir()
Second, restore dependency info from dependency file by GetDepFile()
Third, do Publishing by doPublish() function.
in doPublish() function, every file will be opened again and add html head, html end. The angel brackets 
will be substituted to by ChangeBracket() function.


Public Interface:
=================

- CodePublisher;				//constructor and get the AST tree from Repository
- GetDir						//get the directory name for publishing html webpages.
- GetDepFile					//get the dependency file and restore the dependency info from it
								// and store the info into myDb
- doPublish;					// do publishing code pages.
- AddHtmlHead					//Add html head in str.
- AddFileName					//get the file name from the file path and add it in str.
- AddHtmlEnd					//add html end in str.
- ChangeBracket					// change the angel bracket characters in file into "&lt;" and "&gt;"
- AddDependencyFiles			//get the dependency file names from myDb and add them in str.

Build Process:
==============
Required files
- CodePubisher.h, CodePublish.cpp, HideFuncHandle.cpp, HideFuncHandle.h

Build commands
- devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.1 : 03 May 2017
- remove the function of publishing menu.
ver 1.0 : 04 April 2017
- first release
*/

#include "../NoSqlDb/NoSqlDb.h"
#include <vector>

#include <string>

namespace CodeAnalysis
{

	class CodePublisher {
	  public:
		//constructor and get the AST tree from Repository
		  CodePublisher();
		//get the directory name for publishing html webpages.
		 void GetDir(std::string dir,std::string, std::string menuFileName = "\\MyMenu0404.htm");
		//get the dependency file and restore the dependency info from it and store the info into myDb
		void GetDepFile(std::string depFile);
		// do publishing code pages.
		std::vector<std::string> doPublish();
		//Add html head in str.
		void AddHtmlHead(std::string &str, std::string path);
		//get the file name from the file path and add it in str.
		void AddFileName(std::string &str, std::string filePath);
		//add html end in str.
		void AddHtmlEnd(std::string &str);
		//get the dependency file names from myDb and add them in str.
		void AddDependencyFiles(std::string & str, std::string title, const NoSQLDB::Keys &, NoSQLDB::Key);
		// change the angel bracket characters in file into "&lt;" and "&gt;"
		void ChangeBracket(std::string &str);
		
	  private:
	//private function.
		//get the file name from the file path
		std::string GetFileNameFromPath(std::string path);
		//open the code file and add the file content in str.
		void AddFileContent(std::string & str, std::string filepath);
		//copy the css file and the javasript file to the directory for storing the webpages
		void CopyCSSandJSfile(std::string);
	//private data:
		// the directory to store the web pages
		std::string saveDir;
		// store the result of dependency analysis.
		NoSQLDB::NoSqlDb<std::string> myDb;
		// menu file path
		std::string menuPath;
		std::string scDir;
	};

}
#endif
