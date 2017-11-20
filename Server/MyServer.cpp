/////////////////////////////////////////////////////////////////////////
// MyServer.cpp-Demonstrates how the server listen the port, recieve the// 
//              message and files and send message and files to client. //
// Yipei Zhu , CSE687 - Object Oriented Design, Spring 2017            //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Windows 10 pro                     //
// ver 1.0                                                             //
/////////////////////////////////////////////////////////////////////////

#include "MyServer.h"
#include ".\MsgServer.h"
#include "..\PublishRepository\PublishRepository.h" 
#include <sstream>

using namespace Utilities;
using namespace Async;

//destructor: to delete the class instance
MyServer::~MyServer() {

	for (auto item : pClientList) {
		if (item.second != NULL) {
			item.second->stop();
		}

	}

	for (auto itemMsg : pMsgList) {
		if (itemMsg.second != NULL) {
			delete itemMsg.second;
		}

	}
}
//send the message to the queue according to the client numebr
void MyServer::enQByClientNum(int clientNum,Message pmsg) {
	if (pMsgList.find(clientNum) != pMsgList.end()) {
		pMsgList[clientNum]->enQ(pmsg);
	}
}
//get the string stream according vector list.
void MyServer::ProcessingVectorList(std::string & str, std::vector<std::string> list) {
	int ctgsize = list.size();
	std::string NumStr = Utilities::Converter<int>::toString(ctgsize);
	str = str + NumStr + "\n";
	for (int i = 0; i < ctgsize; i++)
	{
		str = str + list[i] + "\n";
	}
}
//processing the command of 'download'
std::vector<std::string> MyServer::ProcessingDownLoad(std::string cmd)
{
	std::string fileName = cmd.substr(2, cmd.length() - 2);
	std::vector<std::string> fileList = CodeAnalysis::PublishRepository::getInstance()->getDepfileList(fileName);
	return fileList;
	
}
//processing the body of message which is recieved from client.
std::string MyServer::ProcessingCommand(std::string cmd) {
	std::string sendMsg = "";
	if (cmd[0] == '2')
	{
		sendMsg = "6\n";
		std::vector<std::string> categoryList = CodeAnalysis::PublishRepository::getInstance()->getCategoryList();
		ProcessingVectorList(sendMsg, categoryList);
		
	}else if (cmd[0] == '5') 
	{
		sendMsg = "7\n";
		std::string categoryName = cmd.substr(2, cmd.length() - 3);
		std::vector<std::string> fileList = CodeAnalysis::PublishRepository::getInstance()->getfileListbyCtg(categoryName);
		ProcessingVectorList(sendMsg, fileList);

	}
	else if (cmd[0] == '4')  ///delete file
	{ 
		std::string fileName = cmd.substr(2, cmd.length() - 2);
		CodeAnalysis::PublishRepository::getInstance()->deleteFile(fileName);
	}
	else {
		sendMsg = cmd;
	}
	return sendMsg;
}


//start the server
void MyServer::Start(std::string path) {
	std::cout<<"\n  ----------------------------------Reciever started---------------------------------------------\n";
	BlockingQueue<HttpMessage> msgQ;
	try{	//start the listener for receiving the client's request.
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		ClientHandler cp(msgQ);
		cp.setPublishDir(path);
		sl.start(cp);
		while (true){	// get the request from client
			HttpMessage msg = msgQ.deQ();
			std::string str = msg.bodyString();
			std::string clientUrl = msg.findValue("PortNum");
			int ClientNum = Utilities::Converter<int>::toValue(clientUrl);
			//client's connection request.
			if (str[0] == '1'){
				//int portnum = Utilities::Converter<int>::toValue(str.substr(2, str.length()));
				MyClient * pClient = new MyClient(ClientNum);
				BlockingQueue<Message> *pMsgList = new BlockingQueue<Message>();
				AddClient(ClientNum, pClient);
				AddMsg(ClientNum, pMsgList);
				std::thread t1(	// start the 'send' thread for sending message to the specified client.
					[=]() {
					pClient->execute(*pMsgList, ClientNum);
					delete pClient;
				});
				t1.detach();
			}
			else {	// processing the message from clients.
				std::string cmd = msg.bodyString();
				if (cmd[0] != '8'){
					std::string sendMsg = ProcessingCommand(cmd);
					if (sendMsg != "") {
						enQByClientNum(ClientNum, sendMsg);//send message back to Client.
					}
				}else {
					std::vector<std::string> sendlist = ProcessingDownLoad(cmd);
					for (auto sendItem : sendlist){
						enQByClientNum(ClientNum, "3\n"+sendItem);//send message back to Client.
					}
					enQByClientNum(ClientNum, "9\n");
				}
			}
		}
	}
	catch (std::exception& exc){
		std::cout<<"\n  Exeception caught: " << std::string(exc.what()) << "\n\n";
	}
}

//----< test stub >--------------------------------------------------
#ifdef TEST_SERVER



int main()
{
	MyServer mySvr;
	mySvr.Start("C:\\vs\\project4-1\\Project3_0331\\Testfiles\\Server\\SourceCode");


  
}

#endif