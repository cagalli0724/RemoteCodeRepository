//////////////////////////////////////////////////////////////////////
//  DepAnal.cpp - demonstrate how to analyze the dependency         //
//  ver 1.1                                                         //
//  Language:      Visual C++ 2015                                  //
//  Platform:      Window10                                         //
//  Application:   CSE687 Pr2, Sp17                                 //
//  Author:        Yipei Zhu                                        //
//                 yzhu41@syr.edu                                   //
//////////////////////////////////////////////////////////////////////

#include "DepAnal.h"
#include "../DepDisplay/DepDisplay.h"
#include "../XmlDocument/XmlElement.h"
#include "../Persist/Persist.h"
#include "../Tokenizer/Tokenizer.h"
#include "../XmlDocument/dbitokcollection.h"

using namespace CodeAnalysis;

//constructor
DepAnal::DepAnal() 
{
}
//show the dependency analysis
void DepAnal::depDisplay(int size){

	DepDisplay display(myDb);
	display.showHeader(size);
	display.showAll(size);

}
//save the dependency analysis to files
void DepAnal::saveToFile(std::string dependencyPath) {
	NoSQLDB::Persist<std::string> persist(myDb);
	persist.saveToFile(dependencyPath);
}

//strong component analysis
void DepAnal::SCAnal(std::string scPath) 
{
	sc.clear();
	Keys keys = myDb.keys();
	// add the dependency info into sc.
	for (auto k : keys) 
	{
		sc.addNode(k);    //add node, the value of node is the fileName
		Keys dependencyList = myDb.value(k).children;
		for (auto d : dependencyList) 
		{
			sc.addDependency(k, d);
		}
	}
	std::cout << "\n  ------------------------------ strong component analysis  -----------------------------------------\n";
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	sc.Tarjan();   //Tarjan analysis
	sc.show();
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::cout << "       Strong Component analysis has been stored in "<<scPath << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	sc.writeToXml(scPath);
}

//get FileList by categoryName
std::vector<std::string> DepAnal::getFileListbyCategory(std::string categoryName)
{
	std::vector<std::string> fileList;
	Keys keys = myDb.keys();
	for (auto key : keys) {
		NoSQLDB::Element<std::string> elem = myDb.value(key);
		if ((elem.category == categoryName)||(categoryName == "<...>")) {
			if(elem.category != "")
				fileList.push_back(elem.category+"\\"+elem.data);
			else
				fileList.push_back(elem.data);
		}
	}
	return fileList;
}


//dependency analysis for each file
void DepAnal::DepAnalForFile(std::ifstream &in, NoSQLDB::Element<std::string>& elem,std::string fileName)
{
	std::vector<std::string> children;
	Scanner::Toker toker;
	toker.attach(&in);
	
	while (in.good())
	{                
		//find whether the file has type info which is defined in another file.
		std::string tok = toker.getTok();
	
		if (tok != "#") {
			continue;
		}
		tok = toker.getTok();
		if (tok != "include") {
			continue;
		}
		tok = toker.getTok();
		if (tok.find("\"") == std::string::npos) {
			continue;
		}
		std::string dependencyName = tok.substr(1, tok.length()-2);
		//if (fileName[0] != '.')
		//	fileName = ".\\" + fileName;
		std::string directory = fileName.substr(0, fileName.find_last_of("\\\\")+1);
		std::string path = FileSystem::Path::getFullFileSpec(directory + dependencyName);
		if (!FileSystem::File::exists(path)) {
			std::cout << "\n according to the '#include' Analysis, the file:\n      ";
			std::cout << fileName << " dependents on:\n      ";
			std::cout << path << std::endl;
			std::cout << "    But "<<path<<" hasn't existed in File System!" << std::endl;
			std::cout << "    Please upload " << path << " Later!\n" << std::endl;
		}
		children.push_back(path);		

	} 

	std::sort(children.begin(), children.end()); //erase the duplicate file names
	children.erase(std::unique(children.begin(), children.end()), children.end());
	for (auto item : children) 
		elem.saveChild(item); 
	
}
//delete the file in publish repository.
void DepAnal::deleteFile(std::string path)
{
	if (myDb.hasKey(path)) {
		myDb.removeRecord(path);
	}
}
//get the file list that dependent on the specified file name
void DepAnal::getDepfileList(std::string fileName, std::vector<std::string>& depfileList) {
	
	if (myDb.hasKey(fileName))
	{
		NoSQLDB::Element<std::string> elem = myDb.value(fileName);
		std::string str = elem.category + "\\" + elem.data + ".htm\n" + elem.category;
		std::string strCss = elem.category + "\\" + "Style.css\n" + elem.category;
		std::string strJs = elem.category + "\\" + "func.js\n" + elem.category;
		if (find(depfileList.begin(), depfileList.end(), str) == depfileList.end()) {
			depfileList.push_back(str);
			depfileList.push_back(strCss);
			depfileList.push_back(strJs);
			Keys keys = elem.children;
			for (Key key : keys)
			{
				getDepfileList(key, depfileList);
			}
		}
		
	}
	
}

//prepare for the dependency analysis.
void DepAnal::DependencyAnalysis(std::string fileName,std::string dir) 
{
	int last = fileName.find_last_of("\\");
	NoSQLDB::Element<std::string> elem(fileName, "");
	if (dir.length() != last) {
		std::string tmp = fileName.substr(0, last );
		elem.category = tmp.substr(dir.length()+1, tmp.length());
	}
		
	elem.data = fileName.substr(fileName.find_last_of("\\")+1, fileName.length());
	std::ifstream in(fileName);
	if (!in.good()) 
	{
		std::cout << "\n  can't open " << fileName << "\n\n";
		return;
	}
	DepAnalForFile(in, elem,fileName);
	myDb.saveRecord(elem.name,elem);    //save the file elem to noSqlDb
	

}




//test sub-----------------------------------------------------------------------------------------------
#ifdef  TEST_DEPANAL


int main(int argc, char* argv[])
{
	// collecting tokens from files, named on the command line
	if (argc < 2)
	{
		std::cout<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}
	std::vector<std::string> file;
	for (int i = 1; i<argc; ++i)
	{
		std::string fileSpec = FileSystem::Path::getFullFileSpec(argv[i]);
		std::string msg = "Processing file" + fileSpec;
		file.push_back(fileSpec);
		try
		{
			
			
			DepAnal da;
			da.DependencyAnalysis(".\\DepAnal.cpp",".\\");
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	}
	std::cout << "\n";
	return 0;
}



#endif //  TEST_DEPANAL


