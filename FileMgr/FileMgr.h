#ifndef FILEMGR_H
#define FILEMGR_H
/////////////////////////////////////////////////////////////////////
// FileMgr.h - find files matching specified patterns              //
//             on a specified path                                 //
// ver 2.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a class, FileMgr, and object factory function,
* Create().  It's mission is to search the directory tree, rooted at
* a specified path, looking for files that match a set of specified
* patterns.
*
* It provides virtual functions file(...), dir(...), and done(), which
* an application can override to provide application specific processing
* for the events: file found, directory found, and finished processing.
*
* The package also provides interface hooks that serve the same purpose
* but allow multiple receivers for those events.
*
* Required Files:
* ---------------
*   FileMgr.h, FileMgr.cpp, IFileMgr.h, 
*   FileSystem.h, FileSystem.cpp
*
* Build Process:
* --------------
*   devenv FileMgr.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 2.2 : 28 Aug 2016
* - added more prologue comments
* ver 2.1 : 31 Jul 2016
* - added virtual file, dir, and done functions
* - this lets application override instead of registering for event
* ver 2.0 : 12 Mar 2016
* - fixes bug in directory recursion in find(path)
* - reduces scope of for loop in find for efficiency
* ver 1.0 : 11 Mar 2016
* - first release
*/

#include "IFileMgr.h"
#include "../FileSystem/FileSystem.h"

namespace FileManager
{
  class FileMgr : public IFileMgr
  {
  public:
    using patterns = std::vector<std::string>;
    using File = std::string;
    using Dir = std::string;
    using fileSubscribers = std::vector<IFileEventHandler*>;
    using dirSubscribers = std::vector<IDirEventHandler*>;
    using doneSubscribers = std::vector<IDoneEventHandler*>;
	FileMgr(const std::string& path);
	virtual void FileMgr::file(const File& f);
	void addPattern(const std::string& patt);
	virtual void dir(const Dir& fpath);
	virtual void done();
	void search();
	void find(const std::string& path);
	void regForFiles(IFileEventHandler* pHandler);
	void regForDirs(IDirEventHandler* pHandler);
	void regForDone(IDoneEventHandler* pHandler);

	virtual ~FileMgr();

	static IFileMgr* getInstance()
	{
		return pInstance_;
	}

  private:
	  std::string path_;
	  patterns patterns_;
	  size_t numFilesProcessed = 0;
	  fileSubscribers fileSubscribers_;
	  dirSubscribers dirSubscribers_;
	  doneSubscribers doneSubscribers_;
	  static IFileMgr* pInstance_;
  };
}
#endif
