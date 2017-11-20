#ifndef TESTCLIENT_H
#define TESTCLIENT_H

/////////////////////////////////////////////////////////////////////////
// testClient.h - test the function of projet 4                        // 
// Yipei Zhu , CSE687 - Object Oriented Design, Spring 2017            //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Windows 10 pro                     //
// ver 1.0                                                             //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements testClient class which is used to test functions
* for project 4.
* It has two test cases which are implemented in testcase1 and testcase2
* functions.
* the two functions has two arguements for initializing the class 
* testcase1: none
* testcase2: + fileNameToAdd + fileNameToDelete
*
* Public Interface:
* =================
*
* - testClient				//constructor
* - start					//start the client
* - stop					//stop the client
*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   MyClient.cpp, MyClient.h
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*   testClient.h, testClient.cpp
*
*	Build commands
* - devenv Project3_0331.sln /rebuild debug
*
*   Maintenance History:
*	====================
*	ver 1.0 : 03 May 2017
*	- first release
*/



class testClient
{
public:
	//constructor
	testClient(int number, std::string pathToAdd, std::string pathToDelete) :
		Cl(number), pathToAdd_(pathToAdd), pathToDelete_(pathToDelete) {
		num = number;
	}
	//start the client
	void start();
	//stop the client.
	void stop();
private: 
//private function:	
	//get the publish file to launch the file 
	std::string getPublishFile(int ClientNum);
	//processing the command for client
	void ProcessingCommand(std::string cmd);
	//for test case 1
	void testcase1();
	//for test case 2
	void testcase2();
//private data:
	//myClient instance
	MyClient Cl;
	//for test case 1 to store the category.
	std::vector<std::string> categories;
	//the file the store the dowloaded files
	std::string dir;
	//the file to add
	std::string pathToAdd_;
	//the file to delete
	std::string pathToDelete_;
	// the number of client.
	int num;
	// the sign to stop
	bool stop_;
};



#endif
