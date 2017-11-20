/////////////////////////////////////////////////////////////////////
// FileMgr.cpp - find files matching specified patterns            //
//               on a specified path                               //
// ver 2.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
/////////////////////////////////////////////////////////////////////

#include "FileMgr.h"
#include <iostream>

using namespace FileManager;

FileMgr::~FileMgr()
{
}

//----< set default file pattern >-------------------------------

FileMgr::FileMgr(const std::string& path) 
	: path_(path)
{
	patterns_.push_back("*.*");
	pInstance_ = this;
}
//----< return instance of FileMgr >-----------------------------


//----< add file pattern, removing default on first call >-------

void FileMgr::addPattern(const std::string& patt)
{
	if (patterns_.size() == 1 && patterns_[0] == "*.*")
		patterns_.pop_back();
	patterns_.push_back(patt);
}
//----< applications can overload this or reg for fileEvt >------

void FileMgr::file(const File& f)
{
	++numFilesProcessed;
	for (auto pEvtHandler : fileSubscribers_)
	{
		pEvtHandler->execute(f);
	}
}
//----< applications can overload this or reg for dirEvt >-------

void FileMgr::dir(const Dir& fpath)
{
	for (auto pEvtHandler : dirSubscribers_)
	{
		pEvtHandler->execute(fpath);
	}
}
//----< applications can overload this or reg for doneEvt >------

void FileMgr::done()
{
	for (auto pEvtHandler : doneSubscribers_)
	{
		pEvtHandler->execute(numFilesProcessed);
	}
}
//----< start search on previously specified path >--------------

void FileMgr::search()
{
	find(path_);
	done();
}
//----< search current path including subdirectories >-----------

void FileMgr::find(const std::string& path)
{
	std::string fpath = FileSystem::Path::getFullFileSpec(path);
	dir(fpath);
	for (auto patt : patterns_)
	{
		std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
		for (auto f : files)
		{
			file(f);
		}
	}
	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;
		find(dpath);
	}
}
//----< applications use this to register for notification >-----

void FileMgr::regForFiles(IFileEventHandler* pHandler)
{
	fileSubscribers_.push_back(pHandler);
}
//----< applications use this to register for notification >-----

void FileMgr::regForDirs(IDirEventHandler* pHandler)
{
	dirSubscribers_.push_back(pHandler);
}
//----< applications use this to register for notification >-----

void FileMgr::regForDone(IDoneEventHandler* pHandler)
{
	doneSubscribers_.push_back(pHandler);
}
  

IFileMgr* FileMgrFactory::create(const std::string& path)
{
	  return new FileMgr(path);
  }

IFileMgr* FileMgr::pInstance_;

IFileMgr* IFileMgr::getInstance()
{
  return FileMgr::getInstance();
}

#ifdef TEST_FILEMGR

using namespace FileManager;

struct FileHandler : IFileEventHandler
{
  void execute(const std::string& fileSpec)
  {
    std::cout << "\n  --   " << fileSpec;
  }
};

struct DirHandler : IDirEventHandler
{
  void execute(const std::string& dirSpec)
  {
    std::cout << "\n  ++ " << dirSpec;
  }
};

struct DoneHandler : IDoneEventHandler
{
  void execute(size_t numFilesProcessed)
  {
    std::cout << "\n\n  Processed " << numFilesProcessed << " files";
  }
};
int main()
{
  std::cout << "\n  Testing FileMgr";
  std::cout << "\n =================";

  std::string path = FileSystem::Path::getFullFileSpec("..");
  IFileMgr* pFmgr = FileMgrFactory::create(path);

  FileHandler fh;
  DirHandler dh;
  DoneHandler dnh;

  pFmgr->regForFiles(&fh);
  pFmgr->regForDirs(&dh);
  pFmgr->regForDone(&dnh);

  pFmgr->addPattern("*.h");
  pFmgr->addPattern("*.cpp");
  //pFmgr->addPattern("*.log");

  pFmgr->search();

  std::cout << "\n\n";
  return 0;
}
#endif
