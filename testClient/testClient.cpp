/////////////////////////////////////////////////////////////////////////
// testClient.cpp - test the function of projet 4                      // 
// Yipei Zhu , CSE687 - Object Oriented Design, Spring 2017            //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Windows 10 pro                     //
// ver 1.0                                                             //
/////////////////////////////////////////////////////////////////////////

#include "..\MockChannel\MsgClient.h"
#include "..\MockChannel\MyClient.h"
#include "testClient.h"
#include "..\Utilities\Utilities.h"
#include "..\Logger\Cpp11-BlockingQueue.h"
#include "..\Server\MsgServer.h"
#include "..\FileSystem\FileSystem.h"

#include<stdlib.h>

using Message = std::string;


//get publish file for GUI. 
//the default folder is .\Client + ClientNum +\lazyDownLoad
std::string testClient::getPublishFile(int ClientNum) {
	std::string str = Utilities::Converter<int>::toString(ClientNum);
	str = ".\\Client" + str + "\\lazyDownLoad";
	str = FileSystem::Path::getFullFileSpec(str);
	if (!FileSystem::Directory::exists(str)) {
		FileSystem::Directory::createAll(str);
	}
	return str;
}
//processing the command from server
void testClient::ProcessingCommand(std::string cmd)
{
	if ((cmd[0] == '6') || (cmd[0] == '7'))  // get the categories for files from server
	{
		static int staticindex = 0;
		if (cmd[0] == '6') {
			std::cout << "\n  ------------------------------Display file categories --------------------------------------------\n    ";
		}			
		if (cmd[0] == '7') {
			std::cout << "\n  -------Display all files of "<<categories[staticindex]<<" from remote server(<...> means all files)--------\n";
			staticindex = staticindex + 1;
		}		
		std::string catgNameList = cmd.substr(2, cmd.length());
		int index = catgNameList.find_first_of("\n");
		std::string catNum = catgNameList.substr(0, index);
		int catNumInt = Utilities::Converter<int>::toValue(catNum);
		catgNameList = catgNameList.substr(index + 1, catgNameList.length());

		for (int i = 0; i < catNumInt; i++)
		{
			std::string catgName = catgNameList.substr(0, catgNameList.find_first_of("\n"));;
			catgNameList = catgNameList.substr(catgNameList.find_first_of("\n") + 1, catgNameList.length());
			if (catgName == "") {
				std::cout << "   <No parent>\n";
			}
			else {
				std::cout << "   "<<catgName << "\n";

			}
			
		}
		std::cout << "\n  --------------------------------------------------------------------------------------------------\n";

	}
	if (cmd[0] == '9')	//the lazy downloading has completed, open the web page locallys
	{
		std::cout<< "\n  ------------------------------------  Lazy Download  -----------------------------------------------\n";
		std::cout << "\n  I will open the file: Child\\Child.cpp. It will open by internet. The related file: Child\\Child.h" << std::endl;
		std::cout << "\n  also will be downloaded. You can visit it locally. On the other hand, javascript and css file also" << std::endl;
		std::cout << "\n  also will be download. The local directory is:"<< dir << std::endl;
		std::string command("start \"\" \"" + dir + "\\"+ "Child\\Child.cpp.htm" + "\"");
		std::system(command.c_str());
	}


}

//----< signal server thread to stop >---------------------------------------

void testClient::stop()
{
	Cl.stop();
	stop_ = true;
}
//for test case1
void testClient::testcase1(){
	std::cout << "\n  ------------------------------ Client Test case1 --------------------------------------------------\n";
	std::cout << "\n  In test Case 1, the client will test the function: display the category and files on Remote " << std::endl;
	std::cout << "\n  Repository, lazy download the files from remote repository.  " << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	Async::BlockingQueue<Message> sendQ;
	std::thread t1(
		[&]() {
		Cl.execute(sendQ, 8080);
	});	
	//start the listener on client to recieve the message.
	Async::BlockingQueue<HttpMessage> httpMsgQ;
	SocketSystem ss;
	SocketListener sl(Cl.getClientNum(), Socket::IP6);
	ClientHandler ch(httpMsgQ);
	int clientNum = Cl.getClientNum();
	dir = getPublishFile(clientNum);
	ch.setPublishDir(dir);
	sl.start(ch);
	//thread for recieving files and message from server
	std::thread t2(
		[&]() {
		SocketSystem ss;
		while (true) {
			HttpMessage msg = httpMsgQ.deQ();
			if (msg.bodyString() == "quit" || stop_ == true) {
				std::cout << "The reciever of client will stop!" << std::endl;
				this->stop();
				sl.stop();
				break;
			}
			this->ProcessingCommand(msg.bodyString());
		}
	});
	// test to connect with server. So the server can send message back to client.
	sendQ.enQ("1\n");
	Sleep(1000);
	sendQ.enQ("2\n");
	//test to get files according to the category
	categories = {"","Child","<...>" };
	for (auto category : categories) {
		sendQ.enQ("5\n"+ category+ "\n");
	}
	//test lazy download
	sendQ.enQ("8\nChild\\Child.cpp");	
	Sleep(20000);
	sendQ.enQ("quit");
	t1.join();
	t2.join();
}
//for test cases 2
void testClient::testcase2() {
	std::cout << "\n  ------------------------------ Client Test case2 --------------------------------------------------\n";
	std::cout << "\n  In test Case 2, the client will test the function: upload, delete files!" << std::endl;
	std::cout << "\n  ---------------------------------------------------------------------------------------------------\n";
	std::string fullPath = FileSystem::Path::getFullFileSpec(pathToAdd_);
	if (!FileSystem::File::exists(fullPath))
	{
		std::cout << "   you give the wrong path of file to add!" << std::endl;
		return;
	}
	Async::BlockingQueue<Message> sendQ;
	std::thread t1(
		[&]() {
		Cl.execute(sendQ, 8080);
	});
	//start the listener on client to recieve the message.
	Async::BlockingQueue<HttpMessage> httpMsgQ;
	SocketSystem ss;
	SocketListener sl(Cl.getClientNum(), Socket::IP6);
	ClientHandler ch(httpMsgQ);
	int clientNum = Cl.getClientNum();
	dir = getPublishFile(clientNum);
	ch.setPublishDir(dir);
	sl.start(ch);//thread for recieving files and message from server
	std::thread t2(
		[&]() {
		SocketSystem ss;
		while (true) {
			HttpMessage msg = httpMsgQ.deQ();
			if (msg.bodyString() == "quit" || stop_ == true) {
				std::cout << "The reciever of client will stop!" << std::endl;
				this->stop();
				break;
			}
			this->ProcessingCommand(msg.bodyString());
		}
	});
	sendQ.enQ("1\n");
	Sleep(1000);
	sendQ.enQ("3\n" + fullPath+"\nChild2");
	std::cout << "\n  ----------------------------------- Upload file ---------------------------------------------------\n";
	std::cout << "\n  The file to upload:\n  "<<fullPath << std::endl;
	std::cout << "\n  ----------------------------------- Delete file ---------------------------------------------------\n";
	std::cout << "\n  The file to delete:" << pathToDelete_ << std::endl;
	sendQ.enQ("4\n" + pathToDelete_);
	Sleep(5000);
	sendQ.enQ("quit");
	t1.join();
	t2.join();
}

//start the test case
void testClient::start() {
	if (num%2 == 1) {   //if the number is odd number, do test case 1
		testcase1();
	}
	else {	//if the number is even number, do test case 2
		testcase2();
	}
}


#ifdef TEST_TESTCLIENT

int main(int argc, char* argv[]) {
	if (argc < 2)
	{
		return 1;
	}
	try
	{	//processing the command arguements.
		std::string numStr = argv[1];
		std::string pathToAdd = "";
		std::string pathToDelete = "";
		int Num = Utilities::Converter<int>::toValue(numStr);
		if (Num%2 != 1) {
			pathToAdd = argv[2];
			pathToDelete = argv[3];
		}
		testClient tc(Num, pathToAdd, pathToDelete);
		tc.start();
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:" << "\n  " << ex.what() << "\n\n";
		return 1;
	}
	std::cout << "\n  You can input any key to quit" << std::endl;
	std::getchar();
	return 0;
}


#endif