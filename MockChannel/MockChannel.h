#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
///////////////////////////////////////////////////////////////////////////////////////
// MockChannel.h - Demo for CSE687 Project #4, Spring 2015                           //
// - build as static Library showing how C++\CLI client can use native code channel  //
// - MockChannel reads from sendQ and writes to recvQ                                //
//                                                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015                         //
// Yipei Zhu                                                                         //
// ver 2.0                                                                           //
///////////////////////////////////////////////////////////////////////////////////////

/*
* This package builds as static Library showing how C++\CLI client can use native code channel  
* MockChannel reads from sendQ and writes to recvQ. There are three class: Sender, Recvr and
* MockChannel. This is the implementation of C++\CLI channel.
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
*   MockChannel.h, MockChannel.cpp
*
*	Build commands
* - devenv Project3_0331.sln /rebuild debug
*
*   Maintenance History:
*	====================
*   ver 2.0 : 02 May 2017
*   - and the sender and recv of socket.
*   - change by student
*	ver 1.0 : by professor
*	- first release
*/
#include "..\HttpMessage\HttpMessage.h"
#include "..\Logger\Cpp11-BlockingQueue.h"
#include ".\Export.h"

using BQueue = Async::BlockingQueue < Message >;
/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts messages from client for consumption by MockChannel
//
class Sendr : public ISendr
{
public:
	void postMessage(const Message& msg);
	BQueue& queue();
private:
	BQueue sendQ_;

};


#endif


