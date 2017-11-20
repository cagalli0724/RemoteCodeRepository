///////////////////////////////////////////////////////////////////////
//  PublishRepository.cpp - provide a repository that provide        //
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

#include "PublishRepository.h"
#include <iostream>
using namespace CodeAnalysis;

//get source cpp file list from CodeAnalysisExecutive Classes and store them into files_ vector
void PublishRepository::getCppFileList(std::vector<File> filesList)
{
	for (auto fileName : filesList)
	{
		cppImplemFiles_.push_back(fileName);
	}
}

//get source cpp file list from CodeAnalysisExecutive Classes and store them into files_ vector
void PublishRepository::gethFileList(std::vector<File> filesList)
{
	for (auto fileName : filesList)
	{
		cppHeaderFiles_.push_back(fileName);
	}
}
//static member initialize
PublishRepository * PublishRepository::instance = NULL;
//strong component analysis
void PublishRepository::SCAnal(std::string scPath) 
{
	da.SCAnal(scPath);
}
//contructor
PublishRepository::PublishRepository()
{
	
	instance = this;
}

// add file from client into repository
void PublishRepository::AddFileInResp(std::string path) {
	std::cout << "\n  The Repository has got a file which is uploaded by Client! " << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::cout << "\n  The file path is: "<<path<<"\n";
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	// dependency analysis for the added file
	da.DependencyAnalysis(path, AnalysisPath_);
	std::cout << "\n  The Repository will do analysis again! " << std::endl;
	std::cout << "\n  The table only shows the names of the files because the paths of files are too long." << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::cout << "\n  ----------------------------------- dependency analysis -------------------------------------------\n";
	da.depDisplay(20);       //display the dependency analysis result
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::cout << "      dependency analysis has been stored in " << dependencyPath_ << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	da.saveToFile(dependencyPath_);
	//strong component Analysis
	SCAnal(componentFile_);
	CodePublisher cp;
	// republish the files
	cp.GetDir(publishDir_, AnalysisPath_);
	cp.GetDepFile(dependencyPath_);
	categoryList_ = cp.doPublish();
}
//delete the file which is requested by client
void PublishRepository::deleteFile(std::string path)
{
	std::string fullpath = AnalysisPath_+ "\\" + path;
	if (FileSystem::File::exists(fullpath)) {
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		std::cout << "\n  The Repository has got a message to delete file. "<< std::endl;
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		std::cout << "\n  The file path is: " << fullpath << "\n";
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		//delete the file in repository
		da.deleteFile(fullpath);
		std::cout << "\n  The Repository will show the depedency analysis again! " << std::endl;
		std::cout << "\n  The table only shows the names of the files because the paths of files are too long." << std::endl;
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		std::cout << "\n  ----------------------------------- dependency analysis -------------------------------------------\n";
		da.depDisplay(20);       //display the dependency analysis result
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		std::cout << "      dependency analysis has been stored in " << dependencyPath_ << std::endl;
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		da.saveToFile(dependencyPath_);
		//strong component Analysis
		SCAnal(componentFile_);
		FileSystem::File::remove(publishDir_ + "\\"+ path +".htm");
		FileSystem::File::remove(AnalysisPath_ + "\\" + path);
		CodePublisher cp;
		cp.GetDir(publishDir_, AnalysisPath_);
		cp.GetDepFile(dependencyPath_);
		categoryList_ = cp.doPublish();
	}
	
}
//get the Dependency file list for lazy downloading
std::vector<std::string> PublishRepository::getDepfileList(std::string fileName)
{
	std::string fullpath = AnalysisPath_ + "\\" + fileName;
	std::vector<std::string> tmp,filelist;
	if (FileSystem::File::exists(fullpath)) 
	{
		std::cout << "\n  The Repository has got a message to download the file and related files. " << std::endl;
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		std::cout << "\n  The file path is: " << fullpath << "\n";
		std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
		da.getDepfileList(fullpath, tmp);
		std::sort(tmp.begin(), tmp.end()); //erase the duplicate file names
		tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
		for (auto fileName : tmp) {
			filelist.push_back(publishDir_ + "\\"+ fileName);
		}
	}
	return filelist;
}
// get file list by category
std::vector<std::string> PublishRepository::getfileListbyCtg(std::string categoryName) 
{
	return da.getFileListbyCategory(categoryName);
}

//do dependency analysis and strong component analysis
void PublishRepository::InitializeDepAnal(std::string dependencyPath,std::string componentFile,std::string publishFile,std::string dir)
{
	AnalysisPath_ = dir;
	componentFile_ = componentFile;
	publishDir_ = publishFile;
	dependencyPath_ = dependencyPath;
	for (auto fileName : cppHeaderFiles_)    //iterate all the files again to find dependency among files
	{
		da.DependencyAnalysis(fileName,dir);       //actually do dependency analysis
	}
	for (auto fileName : cppImplemFiles_)    //iterate all the files again to find dependency among files
	{
		da.DependencyAnalysis(fileName, dir);       //actually do dependency analysis
	}
	std::cout << "   Please wait a moment! Dependency analysis is based on #include analysis!  " << std::endl;
	std::cout << "   the table only shows the names of the files because the paths of files are too long. " << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::cout << "\n  ----------------------------------- dependency analysis -------------------------------------------\n";
	da.depDisplay(20);       //display the dependency analysis result
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::cout << "      dependency analysis has been stored in " << dependencyPath << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	da.saveToFile(dependencyPath);
	//strong component Analysis
	SCAnal(componentFile);
	CodePublisher cp;
	cp.GetDir(publishFile, dir);
	cp.GetDepFile(dependencyPath);
	categoryList_ = cp.doPublish();

}



#ifdef TEST_PUBLISHREPOSITORY
int main() {
	
	PublishRepository * pRep = new PublishRepository();
	pRep->InitializeDepAnal("dp.xml", "sc.xml", ".\\publishList", "..\\");
		
	pRep->AddFileInResp("PublishRepository.cpp");
	return 0;
}



#endif
