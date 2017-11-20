#ifndef MYCLIENT_H
#define MYCLIENT_H

/////////////////////////////////////////////////////////////////////////
// MyClient.h - Demonstrates how the client connect with server, send  // 
//              message to the sever on one-way                        //
// Yipei Zhu , CSE687 - Object Oriented Design, Spring 2017            //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Windows 10 pro                     //
// ver 1.0                                                             //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
* It inherits from MsgClient class.
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*
* Public Interface:
* =================
*
* - sendFile				//send file to server
* - MyClient				//constructor
* - execute					//execute the client function, set the sendQ and get the connection port
* - stop					// set the stop_ value to true for stop the thread in execute function
* - getClientNum			//get the Client NUm
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
*
*	Build commands
* - devenv Project3_0331.sln /rebuild debug
*  
*   Maintenance History:
*	====================
*	ver 1.0 : 03 May 2017
*	- first release
*/

#include ".\MockChannel.h"
#include ".\MsgClient.h"
#include "..\Logger\Cpp11-BlockingQueue.h"


class MyClient:MsgClient
{

public:
	//send file to server
	bool sendFile(const std::string& filename, Socket& socket, const std::string& Category);
	//constructor
	MyClient(int ClientNum);
	//execute the client function, set the sendQ and get the connection port
	void execute(BQueue& sendQ, int port);
	// set the stop_ value to true for stop the thread in execute function
	void stop() { stop_ = true; }
	//get the Client NUm
	int getClientNum();
private:
	//the value for stop the thread
	bool stop_;
	//the client number as string
	std::string myCountString;
	//the client number as int
	int myCount;
	
};

#endif

