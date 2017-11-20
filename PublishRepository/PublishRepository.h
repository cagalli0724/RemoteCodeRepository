#ifndef PUBLISHREPOSITORY_H
#define PUBLISHREPOSITORY_H
///////////////////////////////////////////////////////////////////////
//  PublishRepository.h - provide a repository that provide          //
//                        functionality to publish as linked webpages//
//                        the contents of a set of C++ source code  //
//                        files.                                    //
//  ver 1.0                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr4, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines PublishRepository class. PublishRepository Class is to store the results of
dependency analysis and publish the code pages in the specified directory. The code pages was 
changed into html webpages by InitializeDepAnal function at first.
First, get the filse for publishing by getCppFileList and gethFileList
Second, initialize the info by InitializeDepAnal function
Third, get info and Add file or delete file by AddFileInReps, deleteFile, getDepfileList function.



Public Interface:
=================

- AddFileInResp			// add file from client into repository
- deleteFile			//delete the file in the repository.
- getDepfileList		// get the dependency file list for lazy download.
- getInstance			//get static instance.
- InitializeDepAnal		//do dependency analysis of initialize File
- PublishRepository		//constructor
- SCAnal				//strong component analysis
- getCppFileList		//get the list of cpp file names which we should analyze
- gethFileList			//get the list of h file names which we should analyze
- getCategoryList		// get the category list for GUI
- getfileListbyCtg		//get the file list according to the specified category.

Build Process:
==============
Required files
- PublishRepository.h, PublishRepository.cpp, DepAnal.cpp, DepAnal.h

Build commands
- devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 04 May 2017
- first release
*/


#include <vector>
#include "..\CodePublisher\CodePublisher.h"
#include "../DepAnal/DepAnal.h"

namespace CodeAnalysis {
	class PublishRepository
	{

	public:
		using File = std::string;
		using Slocs = size_t;
		using SlocMap = std::unordered_map<File, Slocs>;
		
		// add file from client into repository
		void AddFileInResp(std::string path);
		//delete the file in the repository.
		void deleteFile(std::string path);
		// get the dependency file list for lazy download.
		std::vector<std::string> getDepfileList(std::string fileName);
		//get static instance.
		static PublishRepository* getInstance()
		{
			return instance;
		}

		//do dependency analysis of initialize File
		void InitializeDepAnal(std::string,std::string, std::string,std::string);
		//constructor
		PublishRepository();
		//strong component analysis
		void SCAnal(std::string);
		//get the list of cpp file names which we should analyze
		void getCppFileList(std::vector<File>);
		//get the list of h file names which we should analyze
		void gethFileList(std::vector<File>);
		// get the category list for GUI
		std::vector<std::string> getCategoryList() { return categoryList_; }
		//get the file list according to the specified category.
		std::vector<std::string> getfileListbyCtg(std::string categoryName);
	private:

		DepAnal da;
		//store the fileNames which we should analyze.
		std::vector<File> cppHeaderFiles_;
		std::vector<File> cppImplemFiles_;
		//store the path for analysis
		std::string AnalysisPath_;
		//store the path for dependency file
		std::string dependencyPath_;
		//store the path for strong component analysis
		std::string componentFile_;
		//store the directory for publishing
		std::string publishDir_;
		// all the category list 
		std::vector<std::string> categoryList_;
		// the pointer  of static instance
		static PublishRepository* instance;

	};


}



#endif

