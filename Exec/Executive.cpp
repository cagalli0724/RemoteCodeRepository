/////////////////////////////////////////////////////////////////////
// Executive.cpp - Directs Code Analysis                           //
// ver 2.2                                                         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
//              Yipei Zhu                                          //
/////////////////////////////////////////////////////////////////////


#include "Executive.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>


#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../DepAnal/DepAnal.h"
#include "../PublishRepository/PublishRepository.h"
#include "../CodePublisher/CodePublisher.h"
#include "..\Server\MyServer.h"


//using Rslt = Logging::StaticLogger<0>;  // use for application results
//using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
//using Dbug = Logging::StaticLogger<2>;  // use for debug output


										/////////////////////////////////////////////////////////////////////
										// AnalFileMgr class
										// - Derives from FileMgr to make application specific file handler
										//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;


using namespace CodeAnalysis;

//----< initialize application specific FileMgr >--------------------
/*
* - Accepts CodeAnalysisExecutive's path and fileMap by reference
*/
AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
	: FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Ext ext = FileSystem::Path::getExt(fqf);
	Pattern p = "*." + ext;
	fileMap_[p].push_back(fqf);
	++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
	d_ = d;
	++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}
//----< returns number of matched files from search >----------------

size_t AnalFileMgr::numFiles()
{
	return numFiles_;
}
//----< returns number of dirs searched >----------------------------

size_t AnalFileMgr::numDirs()
{
	return numDirs_;
}

/////////////////////////////////////////////////////////////////////
// CodeAnalysisExecutive class
// - 
using Path = std::string;
using Pattern = std::string;
using Patterns = std::vector<Pattern>;
using File = std::string;
using Files = std::vector<File>;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;

//----< initialize parser, get access to repository >----------------

CodeAnalysisExecutive::CodeAnalysisExecutive()
{
	
}
//----< cleanup >----------------------------------------------------

CodeAnalysisExecutive::~CodeAnalysisExecutive()
{
	/*
	*  Nothing to do:
	*  - pParser_ and pRepo_ point to objects that the configure_
	*    destructor will delete.
	*  - The AbstractSynTree object will cleanup its contents when
	*    it goes out of scope by deleting the root of the AST node
	*    tree.
	*  - Each node deletes its children, so, again, nothing more
	*    to do.
	*  - This is here to present these comments and to make this
	*    base destructor virtual.
	*/
}
//----< usage message >----------------------------------------------

void showUsage()
{
	std::ostringstream out;
	out << "\n  Usage:";
	out << "\n  Command Line Arguments are:";
	out << "\n  - 1st: path to subdirectory containing files to analyze";
	out << "\n  - remaining non-option arguments are file patterns, e.g., *.h and/or *.cpp, etc.";
	out << "\n  - must have at least one file pattern to specify what to process";
	out << "\n  - option arguments have the format \"\"/x\" , where x is one of the options:";
	out << "\n    - p : set the directory to publishing the code. The directory is also the IIS directory";
	out << "\n          for example: /p HttpCode ";
	out << "\n    - z : set the paths of dependency analysis and strong component.";
	out << "\n          for example: /z dependency.xml component.xml ";
	out << "\n  A metrics summary is always shown, independent of any options used or not used";
	out << "\n\n";
	std::cout << out.str();
	//Rslt::write(out.str());
	//Rslt::flush();
}


//----< show command line arguments >--------------------------------

void CodeAnalysisExecutive::showCommandLineArguments(int argc, char* argv[])
{
	std::ostringstream out;
	out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
	for (int i = 2; i < argc - 1; ++i)
		out << argv[i] << ", ";
	out << argv[argc - 1];
	std::cout <<out.str();
	//Rslt::flush();
}
//processing the commandLine: /p xxx
bool CodeAnalysisExecutive::ProcessPCommandLine(bool& isHasPath, char* argv[])
{
	publishDir_ = FileSystem::Path::getFullFileSpec(argv[0]);
	if (!FileSystem::Directory::exists(publishDir_))
	{
		std::cout << "\n\n  directory to publish the webpages: \"" << publishDir_ << "\" does not exist\n\n";
		return false;
	}
	isHasPath = true;
	return true;
}
//processing the commandLine: /p xxx /z xxx xxx
bool CodeAnalysisExecutive::ProcessingCommandLineForZP(bool &isHasPath, int &i, char* argv[])
{
	if (argv[i][1] == 'z')
	{
		if (!ProcessZCommandLine(isHasPath, &argv[i + 1]))
			return false;
		i = i + 2;
	}
	if (argv[i][1] == 'p')
	{
		if (!ProcessPCommandLine(isHasPath, &argv[i + 1]))
			return false;
		i = i + 1;
	}
	return true;
}
//----< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
* - options: /m (show metrics), /s (show file sizes),/f (store info into log file) and /a (show AST) 
* -          /z xxx.xml xxx.xml (set the paths of dependency analysis and strong component.)
* -          /p xxx(set the directory to publishing the webpages)
*/
bool CodeAnalysisExecutive::ProcessCommandLine(int argc, char* argv[])
{

	if (argc < 2)
	{
		showUsage();
		return false;
	}
	try
	{
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_)) {
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		bool isHasPath = false;
		for (int i = 2; i < argc; ++i) {
			if (argv[i][0] == '/')
			{
				options_.push_back(argv[i][1]);
				if (!ProcessingCommandLineForZP(isHasPath, i, argv))
					return false;

			}
			else {

				patterns_.push_back(argv[i]);
			}
		}
		if (!isHasPath)
		{
			//set default path for storing the result depedency analysis and strong component analysis
			dependencyPath_ = FileSystem::Path::getFullFileSpec(".\\dependency.xml");
			scPath_ = FileSystem::Path::getFullFileSpec(".\\component.xml");
		}
		if (patterns_.size() == 0)
		{
			showUsage();
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:" << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;


}
//processing the commandLine: /z xxx
bool CodeAnalysisExecutive::ProcessZCommandLine(bool& isHasPath, char* argv[])
{
	dependencyPath_ = FileSystem::Path::getFullFileSpec(argv[0]);
	if (!FileSystem::Directory::exists(dependencyPath_.substr(0, dependencyPath_.find_last_of("\\")))) {
		std::cout << "\n\n  path of dependency analysis: \"" << dependencyPath_ << "\" does not exist\n\n";
		return false;
	}
	scPath_ = FileSystem::Path::getFullFileSpec(argv[1]);
	if (!FileSystem::Directory::exists(scPath_.substr(0, dependencyPath_.find_last_of("\\")))) {
		std::cout << "\n\n  path of strong component analysis: \"" << scPath_ << "\" does not exist\n\n";
		return false;
	}
	isHasPath = true;
	return true;
}

//----< returns path entered on command line >-------------------

std::string CodeAnalysisExecutive::getAnalysisPath()
{
	return path_;
}
//----< returns reference to FileMap >---------------------------
/*
* Supports quickly finding all the files found with a give pattern
*/
FileMap& CodeAnalysisExecutive::getFileMap()
{
	return fileMap_;
}
//----< searches path for files matching specified patterns >----
/*
* - Searches entire diretory tree rooted at path_, evaluated
*   from a command line argument.
* - Saves found files in FileMap.
*/
void CodeAnalysisExecutive::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
	numFiles_ = fm.numFiles();
	numDirs_ = fm.numDirs();
}
//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}
//----< retrieve from fileMap all files matching *.h >-----------

std::vector<File> CodeAnalysisExecutive::cppHeaderFiles()
{
	cppHeaderFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h"))
		{
			for (auto file : item.second)
				cppHeaderFiles_.push_back(file);
		}
	}
	return cppHeaderFiles_;
}
//----< retrieve from fileMap all files matching *.cpp >---------

std::vector<File> CodeAnalysisExecutive::cppImplemFiles()
{
	cppImplemFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cpp"))
		{
			for (auto file : item.second)
				cppImplemFiles_.push_back(file);
		}
	}
	return cppImplemFiles_;
}



//----< report number of files processed >-----------------------

size_t CodeAnalysisExecutive::numFiles()
{
	return numFiles_;
}
//----< report number of directories searched >------------------

size_t CodeAnalysisExecutive::numDirs()
{
	return numDirs_;
}
//----< show processing activity >-------------------------------



void CodeAnalysisExecutive::clearActivity()
{
	
}
//----< parses code and saves results in AbstrSynTree >--------------
/*
* - Processes C++ header files first to build AST with nodes for
*   all public classes and structs.
* - Then processes C++ implementation files.  Each member function
*   is relocated to its class scope node, not the local scope.
* - Therefore, this ordering is important.
* - C# code has all member functions inline, so we don't need to
*   do any relocation of nodes in the AST.  Therefore, that analysis
*   can be done at any time.
* - If you bore down into the analysis code in ActionsAndRules.h you
*   will find some gymnastics to handle template syntax.  That can
*   get somewhat complicated, so there may be some latent bugs there.
*   I don't know of any at this time.
*/


//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on name values
* - displayMetrics(...) uses these functions to organize metrics display
*/
static void removeExt(std::string& name)
{
	size_t extStartIndex = name.find_last_of('.');
	name = name.substr(0, extStartIndex);
}


//----< display header line for displayMmetrics() >------------------

void CodeAnalysisExecutive::displayHeader()
{
	std::ostringstream out;
	out << std::right;
	out << "\n ";
	out << std::setw(25) << "file name";
	out << std::setw(12) << "type";
	out << std::setw(35) << "name";
	out << std::setw(8) << "line";
	out << std::setw(8) << "size";
	out << std::setw(8) << "cplx";
	out << std::right;
	out << "\n  ";
	out << std::setw(25) << "-----------------------";
	out << std::setw(12) << "----------";
	out << std::setw(35) << "---------------------------------";
	out << std::setw(8) << "------";
	out << std::setw(8) << "------";
	out << std::setw(8) << "------";
	std::cout<<out.str();
}



//----< comparison functor for sorting SLOC display >----------------

struct compFiles
{
private:
	std::string ChangeFirstCharOfExt(const std::string& fileName) const
	{
		std::string temp = fileName;
		size_t pos = temp.find_last_of('.');
		if (pos < temp.size() - 1)
			if (temp[pos + 1] == 'h')
				temp[pos + 1] = 'a';
		return temp;
	}
public:
	bool operator()(const std::string& fileName1, const std::string& fileName2) const
	{
		return ChangeFirstCharOfExt(fileName1) < ChangeFirstCharOfExt(fileName2);
	}
};




//get publishing directory
std::string CodeAnalysisExecutive::GetPublishDir()
{
	return publishDir_;
}


//----< open file stream for logging >-------------------------------
/*
*  - must come after CodeAnalysisExecutive::processCommandLine()
*  - must come before starting any of the loggers
*/
void CodeAnalysisExecutive::setLogFile(const File& file)
{
	std::string path = getAnalysisPath();
	path += "\\" + file;
	pLogStrm_ = new std::ofstream(path);
	if (pLogStrm_->good())
	{
		//Rslt::attach(pLogStrm_);
		//Demo::attach(pLogStrm_);
		//Dbug::attach(pLogStrm_);
	}
	else
		std::cout<<"\n  couldn't open logFile.txt for writing\n";
}

std::string CodeAnalysisExecutive::systemTime()
{
	time_t sysTime = time(&sysTime);
	char buffer[27];
	ctime_s(buffer, 27, &sysTime);
	buffer[24] = '\0';
	std::string temp(buffer);
	return temp;
}

//----< conduct code analysis >--------------------------------------

#include <fstream>

using namespace CodeAnalysis;

int main(int argc, char* argv[])
{
	CodeAnalysisExecutive exec;
	try
	{   //first, processing the commandline arguments.
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		std::cout << "\n  ---------------------------------------------Initializing---------------------------------------------------\n";
		std::ostringstream tOut("CodeAnalysis");
		Utilities::StringHelper::sTitle(tOut.str(), 3, 92, tOut, '=');
		//Rslt::write(tOut.str());
		std::cout<<"\n     " + exec.systemTime();
		exec.showCommandLineArguments(argc, argv);
		std::cout<<"\n";
		//create the publsih Repository
		PublishRepository* pPubRepo = new PublishRepository();
		//iterate the directory and get the initialized file list.
		exec.getSourceFiles();
		pPubRepo->gethFileList(exec.cppHeaderFiles());
		pPubRepo->getCppFileList(exec.cppImplemFiles());
		std::cout<<"\n";
		std::ostringstream out;
		out << "\n  " << std::setw(10) << "searched" << std::setw(6) << exec.numDirs() << " dirs";
		out << "\n  " << std::setw(10) << "processed" << std::setw(6) << exec.numFiles() << " files";
		std::cout<<out.str()<<std::endl;
		//initialize the file, store the organic dependency relationships in the publishRepository and publish the htm files into publish directory.
		pPubRepo->InitializeDepAnal(exec.GetDependencyPath(), exec.GetScPath(), exec.GetPublishDir(), exec.getAnalysisPath());
		std::cout << "\n  --------------------------------Initialize-----end---------------------------------------------------\n";
		//start the server
		MyServer svr;
		svr.Start(exec.getAnalysisPath());
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		return 1;
	}
	return 0;
}