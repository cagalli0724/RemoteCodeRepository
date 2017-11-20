/////////////////////////////////////////////////////////////////////
// Persist.cpp - store and retrieve NoSqlDb contents               //
// ver 2.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_PERSIST

#include "Persist.h"
#include "../NoSqlDb/Display.h"
#include <string>

using namespace NoSQLDB;


int main()
{
  using Element = Element<std::string>;
  NoSqlDb<std::string> db;
  Element test1("test1", "test");
  test1.data = "test1 string";
  Element test2("test2", "test");
  test2.data = "test2 string";
  test2.saveChild("test1");
  Element test3("test3", "test");
  test3.data = "test3 string";
  test3.saveChild("test1");
  test3.saveChild("test2");
  db.saveRecord("test1", test1);
  db.saveRecord("test2", test2);
  db.saveRecord("test3", test3);
  Display<std::string> display(db);
  display.showAll();
  Persist<std::string> persist(db);
  Persist<std::string>::Xml xml = persist.save();
  std::cout << "\n" << xml << "\n";
  if (persist.saveToFile("DbXml.txt"))
    std::cout << "\n  db saved";
  else
    std::cout << "\n  filesave failed";
  NoSqlDb<std::string> newDb;
  Persist<std::string> persist2(newDb);
  persist2.restore(xml);
  Display<std::string> display2(newDb);
  display2.showAll();
  NoSqlDb<std::string> newerDb;
  Persist<std::string> persist3(newerDb);
  persist3.restoreFromFile("DbXml.Txt");
  Display<std::string> display3(newerDb);
  display3.showAll();
  std::cout << "\n\n";
}
#endif
