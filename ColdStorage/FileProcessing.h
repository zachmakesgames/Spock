#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

#include "Flags.h"


//Stuff for creating the child processes
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <map>

#include "ShaderPack_Public.h"


//Data structures
typedef struct ProcessInfo {
	HANDLE OutputHandle;
	PROCESS_INFORMATION pInfo;
};

class FileProcessor {
public:
	Flags flags;
	//HANDLE SpawnProcess(std::string command);
	struct ProcessInfo SpawnProcess(std::string command);

	void ProcessDirectory(std::string path);
	void DoCommandDir(std::vector<std::string> args);
	void DoCommandFile(std::vector<std::string> args);
	void DoCommandFiles(std::vector<std::string> args);
};

