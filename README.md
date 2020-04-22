# Flow
A dynamic binary instrumentation (DBI) framework

This DBI framework for AMD64 on windows OS allows to instrument process in a multithreaded context. It handle everykind of legitimate binary from .NET to C.


Flow command line : 
```
>FlowInjector.exe 'numberOfDllToInject' 'path/of/the/dll/toinject' 'path/to/the/program/to/instrument' 'the command line of the program to instrument'
```
For instance, if you want to use the default user DLL and inject notepad.exe you can enter : 
```
>FlowInjector.exe 1 FlowUser.dll C:\WINDOWS\system32\notepad.exe
```

Step to use this project

1. To use this project you should have Visual Studio 2019 installed and ready to use.
2. Then you clone the repository 
3. double click on src/FlowInjector/FlowInjector.sln
4. Wait for visual studio 2019 to open the project
5. In visual studio, go in Project->Properties->Debugging, Change the 'Command Arguments' field to your need 
6. Then simply build and run your project. It will automatically run and instrument the target process while considering your user DLL.
7. To debug your userDLL you could use Windbg.


For more informations, read the documentation generated thanks to doxygen located in the doc/html folder.
The most important parts in the doc are located in InstructionCounter/InstructionCounter/*, Flow/FlowExport.h and Flow/FlowInit.h
