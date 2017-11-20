



//////////////////////////////////////////////////////////////////////
//  CodePublisher.cpp - demonstrate how to publish the code pages   //
//  ver 1.1                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include "CodePublisher.h"
#include "./HideFuncHandle.h"
#include "../Persist/Persist.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../FileSystem/FileSystem.h"


using namespace CodeAnalysis;
using namespace NoSQLDB;

//constructor and get the AST tree from Repository
CodePublisher::CodePublisher() { ; }


//get the directory name for publishing html webpages.
void CodePublisher::GetDir(std::string dir, std::string sourcedir, std::string menuFileName)
{
	saveDir = dir;
	scDir = sourcedir;
	menuPath = saveDir + menuFileName;

	std::cout << "  The directory of html pages is:" << saveDir<< std::endl;
	
}

//add head of html webpage
void CodePublisher::AddHtmlHead(std::string &str, std::string path)
{
	//add prologue
	str = str + "<!----------------------------------------------------------------------------\n  ";
	str = str + GetFileNameFromPath(path) + ".htm - for Project #3\n  ";
	str = str + "Published 04 April 2017\n  ";
	str = str + "Yipei Zhu, CSE687 - Object Oriented Design, Spring 2017\n";
	str = str + "----------------------------------------------------------------------------->\n";
	//add html head
	str = str + "<HTML><HEAD><LINK rel=\"stylesheet\" href=\"Style.css\" type=\"text/css\" / >";
	str = str + "<script type=\"text/javascript\" src=\"func.js\"></script></HEAD><body>\n";

}
//get the file name from file path 
std::string CodePublisher::GetFileNameFromPath(std::string path)
{
	std::string fileName = path;
	fileName = fileName.substr(fileName.find_last_of("\\") + 1, fileName.length());
	return fileName;
}
//add FileName into str( str will be the content in the web page)
void CodePublisher::AddFileName(std::string &str, std::string filepath)
{
	str = str + "<h3>" + GetFileNameFromPath(filepath) + "</h3>\n";

}

//get the dependency file and restore the info from the dependency file
void CodePublisher::GetDepFile(std::string depFile)
{
	NoSQLDB::Persist<std::string> persist(myDb);
	persist.restoreFromFile(depFile);

}
//get the dependency file names from myDb and add them in str.
void CodePublisher::AddDependencyFiles(std::string & str, std::string title, const Keys& dependencyFiles, Key DepdFile)
{
	str = str + "<hr / ><div class=\"indent\"><h4>" + title + "</h4>";
	
	
	//iterate the dependency list.
	for (auto depPath : dependencyFiles)
	{
		if (FileSystem::File::exists(depPath)) {
			std::string AddDir = ".\\";
			NoSQLDB::Element<std::string> elem = myDb.value(depPath);
			int i = 0, index = 0;
			while (depPath[i] == DepdFile[i]) {
				if (depPath[i] == '\\')
					index = i;
				i++;
			}
			std::string tmp = DepdFile.substr(index + 1, DepdFile.length());
			std::string targetTmp = depPath.substr(index + 1, depPath.length());
			while (tmp.find_first_of("\\") != std::string::npos) {
				AddDir = AddDir + "..\\";
				tmp = tmp.substr(tmp.find_first_of("\\") + 1, tmp.length());
			}
			AddDir = AddDir + targetTmp;
			str = str + "<a href=\"" + AddDir + +".htm\">" + elem.category + "\\" + elem.data + "</a></br>";

		}
	}
	str = str + "</div><hr / >";

}
// change the angel bracket characters in file into "&lt;" and "&gt;"
void CodePublisher::ChangeBracket(std::string& str)
{
	//change the "<" characters into "&lt;"
	int index = str.find("<");
	while (index != std::string::npos)
	{
		std::string tempEnd = str.substr(index + 1, str.length() - 1);
		str = str.substr(0, index) + "&lt;" + tempEnd;
		index = str.find("<");
	}
	//change the ">" characters into "&gt;"
	index = str.find(">");
	while (index != std::string::npos)
	{
		std::string tempEnd = str.substr(index + 1, str.length() - 1);
		str = str.substr(0, index) + "&gt;" + tempEnd;
		index = str.find(">");
	}

}

//open the code file and add the file content in str.
void CodePublisher::AddFileContent(std::string & str, std::string filepath)
{
	//initialize the hideFuncHandle class
	HideFuncHandle hidefunc;

	str = str + "<div class=\"class1\"><pre>";

	std::ifstream in(filepath);
	std::string s;
	int lineNum = 1;
	while (std::getline(in, s))
	{
		//change the bracket character into &lt; and &gt;
		ChangeBracket(s);
		//modify the line for 'hide' function
		bool ifModify = true;
		std::string s2 = hidefunc.ModifyForHideFunc(lineNum, s, ifModify);
		str = str + s2;
		
		lineNum++;

	}
	str = str + " </pre></div>";
}

//add html end in str
void CodePublisher::AddHtmlEnd(std::string &str)
{
	str = str + "</body></HTML>\n";
}

//copy the css file and the javasript file to the directory for storing the webpages
void CodePublisher::CopyCSSandJSfile(std::string dir)
{
	//copy the css file
	std::string cssFilePath = FileSystem::Path::getFullFileSpec(".\\");
	cssFilePath = cssFilePath + "\\Style.css";
	
	if (!FileSystem::File::exists(cssFilePath))
	{
		std::string throwStr = "path of the css file: " + cssFilePath + " does not exist\n";
		throw std::exception(throwStr.c_str());
	}
	std::string cssFileCpPath = dir + "\\Style.css";
	//std::cout << "  The css file is located at:" << cssFileCpPath << std::endl;
	FileSystem::File::copy(cssFilePath, cssFileCpPath, false);
	//copy the javascript file
	std::string jsFilePath = FileSystem::Path::getFullFileSpec(".\\");
	jsFilePath = jsFilePath + "\\func.js";
	
	if (!FileSystem::File::exists(jsFilePath))
	{
		std::string throwStr = "path of the javascript file: " + jsFilePath + " does not exist\n";
		throw std::exception(throwStr.c_str());
	}
	std::string jsFileCpPath = dir +  "\\func.js";
	//std::cout << "  The javascript file is located at:" << jsFileCpPath << std::endl;
	FileSystem::File::copy(jsFilePath, jsFileCpPath, false);
}

//publishing the webpage according to the NoSqlDb database.
std::vector<std::string> CodePublisher::doPublish()
{
	std::vector<std::string> categoryList;
	CopyCSSandJSfile(saveDir);
	Keys keys = myDb.keys();
	//for every file
	for (auto key : keys)
	{
		std::string str = "";
		//initialize the webpage
		//add the head of html webpage
		AddHtmlHead(str, key);
		AddFileName(str, key);
		NoSQLDB::Element<std::string> elem = myDb.value(key);
		categoryList.push_back(elem.category);
		Keys dependencyList = elem.children;
		std::string title = "Dependencies:";
		AddDependencyFiles(str, title, dependencyList,key);
		AddFileContent(str, key);
		//add the end of html webpage
		AddHtmlEnd(str);
		//save the webpage
		std::string dir = saveDir + "\\" + elem.category;
		if (!FileSystem::Directory::exists(dir)) {
			FileSystem::Directory::createAll(dir);
			CopyCSSandJSfile(dir);
		}
		std::string path = saveDir + "\\" + elem.category +"\\" + elem.data + ".htm";

		std::ofstream out(path);
		if (out.good())
		{
			out << str.c_str();
			out.close();
		}
	}
	
	std::sort(categoryList.begin(), categoryList.end()); //erase the duplicate file names
	categoryList.erase(std::unique(categoryList.begin(), categoryList.end()), categoryList.end());
	return categoryList;
}
////////////////////////////////////////////////////////////
// test sub -----------------------------------------------
#ifdef TEST_CODEPUBLISHER
int main()
{
	CodePublisher cp;
	std::string str;
	std::string fileName = "Z:\\test.h";
	//initialize the webpage
	//add the head of html webpage
	
	cp.AddHtmlHead(str, fileName);
	cp.AddFileName(str, fileName);
	Keys dependencyList;
	std::string fileName2 = "Z:\\test2.h";
	std::string fileName3 = "Z:\\test3.h";
	std::string fileName4 = "Z:\\test4.h";

	dependencyList.push_back(fileName2);
	dependencyList.push_back(fileName3);
	dependencyList.push_back(fileName4);

	std::string title = "Dependencies:";
	cp.AddDependencyFiles(str, title, dependencyList, fileName);
	//add the end of html webpage
	cp.AddHtmlEnd(str);
	std::cout << str << std::endl;

	return 0;
}

#endif
