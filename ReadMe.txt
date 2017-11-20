This file is for run.bat

devenv Project3_0331.sln /rebuild debug
	// This command is for rebuilding the program

start Debug\Exec.exe .\Testfiles\Server\SourceCode /p .\Testfiles\Server\HttpCode /z dp.xml sc.xml *.h *.cpp 
	//This command is for running the server
	Arguments:
	1st: list the directory paths to initialize the dependency files.
	- p : set the directory to publishing the code, this directory also the IIS directory
	for example: /p .\Testfiles\Server\HttpCode
  	- z : set the paths of dependency analysis and strong component.
  	      for example: /z dependency.xml component.xml.

start Debug\testClient.exe 201

	//This command is for running the test unit. there are two test unit.
	//'201' is the port number. If the port number is odd number, the test case 1 will do
	//If the port number is even number, the test case 2 will do

start Debug\testClient.exe 202 .\Testfiles\Client2\Upload\Child2.h Invalid.h
	//the second argument is the file to add
	//the third argument is the file to delete.

start .\Debug\WPF-GUI.exe
	//start the GUI 