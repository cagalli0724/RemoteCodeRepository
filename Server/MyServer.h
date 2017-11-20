#ifndef SERVER_H
#define SERVER_H
/////////////////////////////////////////////////////////////////////////
// MyServer.h - Demonstrates how the server listen the port, recieve the// 
//              message and files and send message and files to client. //
// Yipei Zhu , CSE687 - Object Oriented Design, Spring 2017            //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Windows 10 pro                     //
// ver 1.0                                                             //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that sends HTTP style messages and
* files to a server; and recieve HTTP style messages and files from the clients.
* It has a map of the sender from client.
* The body of Http message has been used to store command:
*	1\n :									connection
*	2\n :									Client send to get Categories for showing
*	3\n + FileName:							Client send to Upload file
*	4\n + FileName:							Client send to Delete File
*	5\n + categoryName\n:		     		Client send to get files of the specified category name  
*	6\n + categoryNum \n + category List\n: Client get files of Category.
*	7\n + fileNum \n + File List\n:		    Client get the file list.
*	8\n + fileName:							Client select the file to download.
*   9\n + fileName:							Client recieve the message that the files has been download from server 
*											to open the file.
*	quit:									quit info
*
* Public Interface:
* =================
*
* - void Start(std::string);				// start the server
*
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
*   server.h, server.cpp
*
*	Build commands
* - devenv Project3_0331.sln /rebuild debug
*
*   Maintenance History:
*	====================
*	ver 1.0 : 04 May 2017
*	- first release
*/

#include <string>
#include <unordered_map>
#include <memory>
#include "../MockChannel/MyClient.h"
#include "../Logger/Cpp11-BlockingQueue.h"
class MyServer
{

public:
	MyServer() { ; }
	~MyServer();
	
	void Start(std::string);
private:
//private function.
	// get the string stream according vector list.
	void ProcessingVectorList(std::string & str, std::vector<std::string> list);
	// add the map of client for sending message according to the recieved message.
	void AddClient(int port, MyClient *pClient) { pClientList[port] = pClient; }
	//processing the body of message which is recieved from client.
	std::string ProcessingCommand(std::string);
	//processing the command of 'download'
	std::vector<std::string> ProcessingDownLoad(std::string cmd);
	//each client has a queue. we should a map between them
	void AddMsg(int port, Async::BlockingQueue<Message> *pMsg) { pMsgList[port] = pMsg; }
	// send the message to the queue according to the client numebr
	void enQByClientNum(int clientNum, Message msg);
//private data:
	// store the map between the client number and MyClient
	std::unordered_map<int,MyClient *> pClientList;
	// store the map between the client number and Queue
	std::unordered_map<int, Async::BlockingQueue<Message> *> pMsgList;
};
#endif