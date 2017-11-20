///////////////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Demo for CSE687 Project #4, Spring 2015                         //
// - build as static Library showing how C++\CLI client can use native code channel  //
// - MockChannel reads from sendQ and writes to recvQ                                //
//                                                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015                         //
// Yipei Zhu                                                                         //
// ver 2.0                                                                           //
///////////////////////////////////////////////////////////////////////////////////////

#define IN_DLL


#include <string>
#include <thread>
#include <iostream>
#include <memory>
#include "..\Server\MsgServer.h"
#include "..\Utilities\Utilities.h"
#include "MockChannel.h"
#include "MsgClient.h"
#include "MyClient.h"

void Sendr::postMessage(const Message& msg)
{
	sendQ_.enQ(msg);
}

BQueue& Sendr::queue() { return sendQ_; }


/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
	Message getMessage();
	BQueue& queue();
private:
	BQueue recvQ_;
};

Message Recvr::getMessage()
{
	return recvQ_.deQ();
}

BQueue& Recvr::queue()
{
	return recvQ_;
}

/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
	MockChannel(ISendr* pSendr, IRecvr* pRecvr);
	void start();
	void stop();
	std::string getPublishFile(int ClientNum);
	void ProcessingCommand(BQueue& recvQ, std::string cmd);
private:

	std::thread thread_;
	ISendr* pISendr_;
	IRecvr* pIRecvr_;
	bool stop_;
	MyClient * pClient;
	SocketListener * pSl;
	ClientHandler *pCh;
	Async::BlockingQueue<HttpMessage> httpMsgQ;
	SocketSystem * pSS;


};

//----< pass pointers to Sender and Receiver >-------------------------------

MockChannel::MockChannel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr), pSl(NULL), pCh(NULL), stop_(false){}


// processing the command from server.
void MockChannel::ProcessingCommand(BQueue& recvQ, std::string cmd)
{
	if ((cmd[0] == '6')||(cmd[0] == '7'))
	{
		if(cmd[0] == '6')
			recvQ.enQ("C");
		if (cmd[0] == '7')
			recvQ.enQ("F");
		std::string catgNameList = cmd.substr(2,cmd.length());
		int index = catgNameList.find_first_of("\n");
		std::string catNum = catgNameList.substr(0, index);
		int catNumInt = Utilities::Converter<int>::toValue(catNum);
		catgNameList = catgNameList.substr(index + 1, catgNameList.length());
		
		for (int i = 0; i < catNumInt; i++)
		{
			std::string catgName = catgNameList.substr(0, catgNameList.find_first_of("\n"));;
			catgNameList = catgNameList.substr(catgNameList.find_first_of("\n") + 1, catgNameList.length());
			recvQ.enQ(catgName);
		}
		recvQ.enQ(".");
		
	}
	if (cmd[0] == '9')
	{
		recvQ.enQ("O");
		recvQ.enQ(pCh->getPublishDir());
	}



}
//get publish file for GUI. 
//the default folder is .\Client + ClientNum +\lazyDownLoad
std::string MockChannel::getPublishFile(int ClientNum) {
	std::string str = Utilities::Converter<int>::toString(ClientNum);
	str = ".\\Client"+str+"\\lazyDownLoad";
	str = FileSystem::Path::getFullFileSpec(str);
	if (!FileSystem::Directory::exists(str)) {
		FileSystem::Directory::createAll(str);
	}
	return str;
}

//----< creates thread to read from sendQ and echo back to the recvQ >-------
void MockChannel::start()
{ //start the listener on client to recieve the message.
	int number = 8081;
	SocketSystem ss;
	pSl = new SocketListener(number, Socket::IP6);//pClient->getClientNum()
	pCh = new ClientHandler(httpMsgQ);
	while (!pSl->start(*pCh)){
		pSl->stop();
		delete pSl;
		number = number + 1;
		pSl = new SocketListener(number, Socket::IP6);//pClient->getClientNum()
	}
	std::string dir = getPublishFile(number);
	pCh->setPublishDir(dir);
	Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
	BQueue& recvQ = pRecvr->queue();
	if (pRecvr == nullptr){
		std::cout << "\n  failed to start Mock Channel (pRecvr)\n\n";
		return;
	}//thread for recieving files and message from server
	std::thread t2(
		[&]() {
		SocketSystem ss;
		while (true) {
			HttpMessage msg = httpMsgQ.deQ();
			if (msg.bodyString() == "quit" || stop_ == true) {
				recvQ.enQ(msg.bodyString());
				this->stop();
				break;
			}
			this->ProcessingCommand(recvQ, msg.bodyString());
		}
	});
	t2.detach();//get send Queue
  Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
  if (pSendr == nullptr){
	  std::cout << "\n  failed to start Mock Channel(pSender)\n\n";
	  return;
  }//this thread is for send message
  pClient = new MyClient(number);
  BQueue& sendQ = pSendr->queue();
  std::thread t1(
		  [&]() { 
	  pClient->execute(sendQ,8080);
	  delete pClient; 
	  } );
  t1.detach(); 
}
//----< signal server thread to stop >---------------------------------------

void MockChannel::stop() 
{
	pClient->stop();
	if (pSl != NULL) {
		pSl->close();
	}
	stop_ = true;
	delete pSl;
	if(pCh != NULL)
		delete pCh;
	if (pSS != NULL) {
		delete pSS;
	}


}

//----< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr) 
{ 
  return new MockChannel(pISendr, pIRecvr); 
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
	
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  Message msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
