#pragma once
/////////////////////////////////////////////////////////////////////
// Display.h - display NoSqlDb contents                            //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines a single class, Display that shows contents
* of an instance of NoSqlDb<Data>.  It accepts a reference to a NoSqlDb<Data>
* database from which it displays records.
*
* I plan to replace the reference with a pointer so that the database can
* be changed without creating a new instance of Display<Data>.
*
* Required Files:
* ---------------
*   Display.h, Display.cpp,
*   NoSqlDb.h, NoSqlDb.cpp, CppProperties.h, CppProperties.cpp
*
* Maintenance History:
*---------------------
* ver 1.0 : 06 Feb 2017
* - first release
*/

#include "../NoSqlDb/NoSqlDb.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>

namespace NoSQLDB
{
  template <typename Data>
  class Display
  {
  public:
    enum Style { summary, detailed };

    Display(NoSqlDb<Data>& db) 
		: db_(db) 
	{
		;
	}

    void show(Key key, Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
    void showAll(Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
    void showHeader(Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
    void showQuery(Keys keys, Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);

  private:
    void showSummary(Key key, size_t fieldwidth, std::ostream& out);
    void showDetails(Key key, size_t fieldwidth, std::ostream& out);
    NoSqlDb<Data>& db_;
  };
  //----< show single record in one-line format >----------------------

  template<typename Data>
  void Display<Data>::showSummary(Key key, size_t fieldwidth, std::ostream& out)
  {
    if (!db_.hasKey(key))
      return;
    Element<Data> elem = db_.value(key);
    int save = out.setf(std::ios::left, std::ios::adjustfield);
    out << "  ";
    out << std::setw(fieldwidth) << key.substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << static_cast<std::string>(elem.name).substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << static_cast<std::string>(elem.category).substr(0, fieldwidth) << " ";
    DateTime dt = elem.dateTime;
    out << std::setw(fieldwidth) << static_cast<std::string>(dt).substr(0, fieldwidth) << " ";
    std::string temp = Utilities::Converter<Data>::toString(elem.data);
    out << std::setw(fieldwidth) << temp.substr(0, fieldwidth) << " ";
    if (elem.children.size() > 0)
    {
      for (Key k : elem.children)
        out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth) << " ";
    }
    out << "\n";
    out.setf(save);
  }
  //----< show single record with detailed, multi-line, format >-------

  template<typename Data>
  void Display<Data>::showDetails(Key key, size_t fieldwidth, std::ostream& out)
  {
    if (!db_.hasKey(key))
      return;
    Element<Data> elem = db_.value(key);
    int save = out.setf(std::ios::left, std::ios::adjustfield);
    out << "  ";
    out << "key      : " << key << "\n  ";
    out << "Name     : " << static_cast<std::string>(elem.name) << "\n  ";
    out << "Category : " << static_cast<std::string>(elem.category) << "\n  ";
    DateTime dt = elem.dateTime;
    out << "DateTime : " << static_cast<std::string>(dt) << "\n  ";
    out << "Data     : " << elem.data << "\n  ";
    out << "Children :\n    ";
    if (elem.children.size() > 0)
    {
      for (Key k : elem.children)
        out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth) << " ";
    }
    out << "\n";
    out.setf(save);
  }
  //----< show single record >-----------------------------------------
  /*
   * Style: Display<Data>::summary or Display<Data>::detailed
   * fieldwidth: number of characters for each field
   * out: stream for output - typically std::cout (default) or std::ostringstream
  */
  template<typename Data>
  void Display<Data>::show(Key key, Style style, size_t fieldwidth, std::ostream& out)
  {
    if (!db_.hasKey(key))
      return;
    if (style == Display<Data>::Style::summary)
      showSummary(key, fieldwidth, out);
    else
      showDetails(key, fieldwidth, out);
  }
  //----< show all records >-------------------------------------------
  /*
  * Style: Display<Data>::summary or Display<Data>::detailed
  * fieldwidth: number of characters for each field
  * out: stream for output - typically std::cout (default) or std::ostringstream
  */
  template<typename Data>
  void Display<Data>::showAll(Style style, size_t fieldwidth, std::ostream& out)
  {
    Keys keys = db_.keys();
    if (style == Display<Data>::Style::summary)
    {
      showHeader(style, fieldwidth, out);
      for (Key k : keys)
        showSummary(k, fieldwidth, out);
    }
    else
    {
      for (Key k : keys)
        showDetails(k, fieldwidth, out);
    }
  }
  //----< show all records in query >----------------------------------
  /*
  * Style: Display<Data>::summary or Display<Data>::detailed
  * fieldwidth: number of characters for each field
  * out: stream for output - typically std::cout (default) or std::ostringstream
  */
  template<typename Data>
  void Display<Data>::showQuery(Keys keys, Style style, size_t fieldwidth, std::ostream& out)
  {
    if (style == Display<Data>::Style::summary)
    {
      showHeader(style, fieldwidth, out);
      for (Key k : keys)
        showSummary(k, fieldwidth, out);
    }
    else
    {
      for (Key k : keys)
        showDetails(k, fieldwidth, out);
    }
  }
  //----< shows underlined names of each field for summary output >----

  template<typename Data>
  void Display<Data>::showHeader(Style style, size_t fieldwidth, std::ostream& out)
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
    out << "  ";
    out << std::setw(fieldwidth) << std::string("Key").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("Name").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("Category").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("TimeDate").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("Data").substr(0, fieldwidth) << " ";
    if (numChildren > 0)
    {
      for (size_t i = 0; i < numChildren; ++i)
        out << std::setw(fieldwidth) << std::string("Child").substr(0, fieldwidth) << " ";
    }
    out << "\n  ";
    for (size_t i = 0; i < numChildren + 5; ++i)
      out << std::setw(fieldwidth) << std::string("----------").substr(0, fieldwidth) << " ";
    out << "\n";
    out.setf(save);
  }
}
