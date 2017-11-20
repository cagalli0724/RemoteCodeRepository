devenv Project3_0331.sln /rebuild debug
start Debug\ServerExec.exe .\Testfiles\Server\SourceCode /p .\Testfiles\Server\HttpCode /z dp.xml sc.xml *.h *.cpp /r
start Debug\testClient.exe 201
start Debug\testClient.exe 202 .\Testfiles\Client2\Upload\Child2.h Invalid.h
timeout /t 10
start .\WPF-GUI\bin\Debug\WPF-GUI.exe

