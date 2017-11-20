#pragma once
/////////////////////////////////////////////////////////////////////
// NoSqlDb.h - key/value pair in-memory database                   //
// ver 1.3														   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
// Yipei														   //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines two classes that support key/value storage:
 * - Element<Data> defines a single value stored in the database.
 *   It provides metadata properties:
 *   - nam
 * - manual information
 * - maintenance information
 * - query mechanism
 * - persistance mechanism
 * - TestExecutive that demonstrates the requirements you meet
 *
 * Required Files:
 * ---------------
 *   NoSqlDb.h, NoSqlDb.cpp, 
 *
 * Maintenance History:
 *---------------------
 * ver 1.3 : 04 April 2017
 * - remove the class Property from Element
 * ver 1.2 : 06 Feb 2017
 * - added children
 * - modified Element<Data>::show()
 * - added these comments
 * ver 1.1 : 28 Jan 2017
 * - fixed bug in NoSqlDb::count() by replacing 
 *   unordered_map<key,Value>::count() with 
 *   unordered_map<key,Value>::size();
 * Ver 1.0 : 25 Jan 2017
 * - first release
 */

#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../NoSqlDb/DateTime.h"

namespace NoSQLDB
{
  using Key = std::string;
  using Keys = std::vector<Key>;
  using Name = std::string;
  using Category = std::string;
  using Children = std::vector<Key>;

  /////////////////////////////////////////////////////////////////////
  // Element class represents a data record in our NoSql database
  // - in our NoSql database that is just the value in a key/value pair
  // - it needs to store child data, something for you to implement
  //
  template<typename Data>
  class Element
  {
  public:

    Element() 
	{
		;
	}
    Element(Name aName, Category aCat) 
		: name(aName), category(aCat) 
	{
		;
	}

    Name name;                  // metadata
    Category category;          // metadata
    DateTime dateTime;          // metadata
    Children children;          // metadata
    Data data;                  // data

    void saveChild(Key);
    std::string show();
  };

  template<typename Data>
  void Element<Data>::saveChild(Key key)
  {
    children.push_back(key);
  }

  template<typename Data>
  std::string Element<Data>::show()
  {
    std::ostringstream out;
    out.setf(std::ios::adjustfield, std::ios::left);
    out << "\n    " << std::setw(8) << "name" << " : " << name;
    out << "\n    " << std::setw(8) << "category" << " : " << category;
    out << "\n    " << std::setw(8) << "dateTime" << " : " << DateTime(dateTime).time();
    Children children_ = static_cast<Children>(children);
    if (children_.size() > 0)
    {
      out << "\n    " << std::setw(8) << "children" << " : ";
      for (size_t i = 0; i < children_.size(); ++i)
      {
        out << children_[i];
        if (i < children_.size())
          out << ", ";
      }
    }
    out << "\n    " << std::setw(8) << "data" << " : " << data;
    out << "\n";
    return out.str();
  }

  /////////////////////////////////////////////////////////////////////
  // NoSqlDb class is a key/value pair in-memory database
  // - stores and retrieves elements
  // - you will need to add query capability
  //   That should probably be another class that uses NoSqlDb
  //
  template<typename Data>
  class NoSqlDb
  {
  public:
    using Key = std::string;
    using Keys = std::vector<Key>;

    Keys keys();
    bool hasKey(Key key);
    bool saveRecord(Key key, Element<Data> elem);
    bool saveValue(Key key, Element<Data> elem);
    bool removeRecord(Key key);
    Element<Data> value(Key key);
    size_t count();
  private:
    using Item = std::pair<Key, Element<Data>>;

    std::unordered_map<Key, Element<Data>> store;
  };

  template<typename Data>
  typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
  {
    Keys keys;
    for (Item item : store)
    {
      keys.push_back(item.first);
    }
    return keys;
  }

  template<typename Data>
  bool NoSqlDb<Data>::hasKey(Key key)
  {
    if (store.find(key) == store.end())
      return false;
    return true;
  }

  template<typename Data>
  bool NoSqlDb<Data>::saveRecord(Key key, Element<Data> elem)
  {
    if (store.find(key) != store.end())
      return false;
    store[key] = elem;
    return true;
  }

  template<typename Data>
  bool NoSqlDb<Data>::removeRecord(Key key)
  {
    if (store.find(key) == store.end())
      return false;
    size_t numErased = store.erase(key);
    if (numErased == 1)
    {
      // remove key from all other element's children collections
      for (Key k : keys())
      {
        Keys& children = store[k].children;  // note Keys& - we don't want copy of children
        Keys::iterator iter;

        for (iter = children.begin(); iter != children.end(); ++iter)
        {

          if ((*iter) == key)
          {
            children.erase(iter);
            break;
          }

        }
      }
      return true;
    }
    return false;
  }

  template<typename Data>
  bool NoSqlDb<Data>::saveValue(Key key, Element<Data> elem)
  {
    if (store.find(key) == store.end())
      return false;
    store[key] = elem;
    return true;
  }

  template<typename Data>
  Element<Data> NoSqlDb<Data>::value(Key key)
  {
    if (store.find(key) != store.end())
      return store[key];
    return Element<Data>();
  }

  template<typename Data>
  size_t NoSqlDb<Data>::count()
  {
    return store.size();
  }
}
