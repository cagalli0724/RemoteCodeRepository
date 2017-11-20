/////////////////////////////////////////////////////////////////////
// DepDisplay.h - display NoSqlDb contents for project2            //
// ver 1.0                                                         //
// Yipei Zhu, CSE687 - Object Oriented Design, Spring 2017         //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines a single class, Display that shows contents
* of an instance of NoSqlDb<std::string>.  It accepts a reference to a NoSqlDb<std::string>
* database from which it displays records.
*
* Public interface:
* - DepDisplay		//Constructor
* - showAll         //show all info stored in NoSqlDb
* - showHeader      //show the header of the table
*
* Required Files:
* ---------------
*   Display.h, Display.cpp,
*   NoSqlDb.h, NoSqlDb.cpp, CppProperties.h, CppProperties.cpp
* Build commands:
* - SET CL=/DTEST_DEPDISPLAY
* - devenv CodeAnalyzerEx.sln /rebuild debug
*
* Maintenance History:
*---------------------
* ver 1.0 : 06 Feb 2017
* - first release
*/


#ifndef DEPDISPLAY_H
#define DEPDISPLAY_H
#include "..\NoSqlDb\NoSqlDb.h"
#include <string>

namespace CodeAnalysis
{
	class DepDisplay
	{
	public:
		using Key = std::string;
		using Keys = std::vector<std::string>;
		//Constructor
		DepDisplay(NoSQLDB::NoSqlDb<std::string>& db);
		//show all info stored in NoSqlDb
		void showAll(size_t fieldwidth = 10, std::ostream& out = std::cout);
		//show the header of the table
		void showHeader(size_t fieldwidth = 10, std::ostream& out = std::cout);
		
	private:
		//show all the info
		void showSummary(Key key, size_t fieldwidth, std::ostream& out);
		//reference of the NoSqlDb
		NoSQLDB::NoSqlDb<std::string>& db_;
	};

}




#endif // !DEPDISPLAY_H





