/////////////////////////////////////////////////////////////////////
// DepDisplay.cpp - display NoSqlDb contents for project2          //
// ver 1.0                                                         //
// Yipei Zhu, CSE687 - Object Oriented Design, Spring 2017         //
/////////////////////////////////////////////////////////////////////


#include "DepDisplay.h"
using namespace CodeAnalysis;
using namespace NoSQLDB;
using StrData = std::string;
using intData = int;
//Constructor
DepDisplay::DepDisplay(NoSQLDB::NoSqlDb<std::string>& db) 
	: db_(db)
{ 
	; 
}
//show all the info
void DepDisplay::showSummary(Key key, size_t fieldwidth, std::ostream& out)
{
	if (!db_.hasKey(key))
		return;
	NoSQLDB::Element<std::string> elem = db_.value(key);
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << "  ";
	int index = key.find_last_of("\\");
	out << std::setw(fieldwidth) << key.substr(index + 1, fieldwidth) << " ";
	out << std::setw(fieldwidth) << elem.category << " ";
	if (elem.children.size() > 0)
	{
		for (Key k : elem.children) 
		{
			int index = k.find_last_of("\\");
			out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(index + 1, fieldwidth) << " ";
		}

	}
	out << "\n";
	out.setf(save);
}
//show the header of the table
void DepDisplay::showHeader(size_t fieldwidth, std::ostream& out)
{
	size_t numChildren = 0;
	Keys keys = db_.keys();
	for (Key k : keys)
	{
		size_t size = db_.value(k).children.size();
		if (numChildren < size)
			numChildren = size;
	}
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << "\n  ";
	out << std::setw(fieldwidth) << std::string("FileName").substr(0, fieldwidth) << " ";
	out << std::setw(fieldwidth) << std::string("Category").substr(0, fieldwidth) << " ";
	if (numChildren > 0)
	{
		for (size_t i = 0; i < numChildren; ++i)
			out << std::setw(fieldwidth) << std::string("dependency file").substr(0, fieldwidth) << " ";
	}
	out << "\n  ---------------------------------------------------------------------------------------------------\n"; 
	out.setf(save);
}
//show all info stored in NoSqlDb
void DepDisplay::showAll(size_t fieldwidth, std::ostream& out)
{
	NoSQLDB::Keys keys = db_.keys();
	//showHeader(fieldwidth, out);
	for (Key k : keys)
		showSummary(k, fieldwidth, out);
}
//test sub --------------------------------------------------------------------
#ifdef TEST_DEPDISPLAY

int main()
{
	std::cout << "\n  Creating and saving NoSqlDb elements with string data";
	std::cout << "\n ------------------------------------------------------------------------------\n";

	NoSqlDb<StrData> db;

	Element<StrData> elem1;
	elem1.name = "elem1";
	elem1.category = "test";
	elem1.data = "elem1's StrData";

	db.saveRecord(elem1.name, elem1);

	Element<StrData> elem2;
	elem2.name = "elem2";
	elem2.category = "test";
	elem2.data = "elem2's StrData";

	db.saveRecord(elem2.name, elem2);

	Element<StrData> elem3;
	elem3.name = "elem3";
	elem3.category = "test";
	elem3.data = "elem3's StrData";

	db.saveRecord(elem3.name, elem3);

	elem1.saveChild(elem2.name);
	elem1.saveChild(elem3.name);
	elem2.saveChild(elem3.name);
	db.saveValue(elem1.name, elem1);
	db.saveValue(elem2.name, elem2);

	DepDisplay display(db);
	display.showHeader(30);
	display.showAll(25);
	std::cout << "\n\n";
	return 0;
}



#endif
