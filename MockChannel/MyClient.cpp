/////////////////////////////////////////////////////////////////////////
// MyClient.cpp - Demonstrates how the client connect with server, send// 
//              message to the sever on one-way                        //
// Yipei Zhu , CSE687 - Object Oriented Design, Spring 2017            //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Windows 10 pro                     //
// ver 1.0                                                            //
/////////////////////////////////////////////////////////////////////////
#include "MyClient.h"
#include <sstream>

//get the Client NUm
int MyClient::getClientNum()
{
	return myCount;
}

//constructor
MyClient::MyClient(int ClientNum) {
	//ClientCounter counter;
	stop_ = false;
	myCount = ClientNum;//(int)counter.count() + 8080; 
	myCountString = Utilities::Converter<int>::toString(myCount);
}

//send file to server
bool MyClient::sendFile(const std::string& filepath, Socket& socket,const std::string & Category)
{
	// assumes that socket is connected and get the file full path
	std::string fileName = filepath.substr(filepath.find_last_of("\\")+1, filepath.length());
	FileSystem::FileInfo fi(filepath);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(filepath);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	//make the httpMessage
	HttpMessage msg = makeMessage(1, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", fileName));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::Attribute("category", Category));    //set the category to server.
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)		//send file by block
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
//execute the client function, set the sendQ and get the connection port
void MyClient::execute(BQueue& sendQ, int port)
{
	
	try
	{	//connect the server
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", port))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		//std::cout<< "client has connected with serverQ"<<std::endl;
		HttpMessage HttpMsg;

		while(!stop_)
		{
			//get the message for sending.
			Message msg = sendQ.deQ();
			if (msg[0] == '3')  ///file upload message.
			{
				//The format is 3\nfilename\ncategoryName
				std::string fileName = msg.substr(msg.find_first_of("\n")+1, msg.find_last_of("\n")-2);
				std::string category = msg.substr(msg.find_last_of("\n") + 1, msg.length());
				//std::cout << "\n  ------------------------------ File Send ----------------------------------------------------------\n";
				//std::cout << "\n  file Name:" << fileName << std::endl;
				//std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
				sendFile(fileName, si,category);
			}
			else {	// for other kind of command, we should send the organic message
				HttpMsg = makeMessage(1, msg.c_str(), myCountString);
				HttpMsg.addAttribute(HttpMessage::Attribute("PortNum", myCountString));
				this->sendMessage(HttpMsg, si);
			}
			// if the msg is quit, quit the thread.
			if (msg == "quit")
			{
				stop_ = true;
				break;
			}
		}
		//std::cout << "the connection of client stops!\n";
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}


#ifdef TEST_MYCLIENT
#include "..\Server\MsgServer.h"
#include <Windows.h>
#include "..\FileSystem\FileSystem.h"

int main()
{

	Async::BlockingQueue<HttpMessage> httpMsgQ;
	
	MyClient *pClient = new MyClient(8081);
	//this thread is for send message
	BQueue sendQ;
	std::thread t1(
		[&]() {
		pClient->execute(sendQ, 8080);
		delete pClient;
	});

	std::string msg = "1\n";
	sendQ.enQ(msg);

	Sleep(1000);

	//msg = "3\nC:\\vs\\project4-1\\Project3_0331\\Testfiles\\Client\\Upload\\Test.cpp\nTest";
	//msg = "2\n";
	std::string publishDir = FileSystem::Path::getFullFileSpec("..\\Testfiles\\Client1\\lazyDownLoad");
	t1.join();
	//delete pCh;
	return 0;
}

#endif