#include "FileProcessing.h"

void FileProcessor::ProcessDirectory(std::string path) {

#ifdef _DEBUG
	std::cout << "Processing directory: " << path << std::endl;
#endif

	std::vector<std::string> files;
	files.clear();

	std::vector<std::string> directories;
	directories.clear();

	std::map<std::string, std::vector<std::string>> fileTable;
	fileTable.clear();

	if (std::filesystem::exists(path)) {
#ifdef _DEBUG
		std::cout << "Path exists" << std::endl;
#endif

		//if the path exists, get all files in the directory and organize by name and extension
		//files with the same prefix will be grouped together and compiled together if possible
		for (std::filesystem::directory_entry de : std::filesystem::directory_iterator(path)) {
#ifdef _DEBUG
			std::cout << "    Processing directory entry" << std::endl;
#endif
			if (de.is_directory()) {
#ifdef _DEBUG
				std::cout << "        Entry is a directory with path: " << de.path().string() << std::endl;
#endif
				directories.push_back(de.path().string());
			}
			else {
#ifdef _DEBUG
				std::cout << "        Entry is a file with path: " << de.path().string() << std::endl;
#endif
				//de.path().stem() gives first part of file name
				//de.path().extension() gives extension
				std::string stem = de.path().stem().string();
				std::string extension = de.path().extension().string();
				std::string filename = de.path().string();

				//check if the file is even a file we care about and if not continue the loop
				if (extension != ".vert" && extension != ".frag" && extension != ".geom" && extension != ".tese" && extension != ".tesc" && extension != ".comp") {
#ifdef _DEBUG
					std::cout << "        ---File with extension " << extension << " is not a shader file" << std::endl;
#endif
					continue;
				}

				if (fileTable.find(stem) != fileTable.end()) {
					//stem exists in the file table so update the table
#ifdef _DEBUG
					std::cout << "        Stem exists in the file table" << std::endl;
#endif
					std::vector<std::string> tempVector = fileTable.find(stem)->second;
					std::vector<std::string>::iterator it;
					it = std::find(tempVector.begin(), tempVector.end(), filename);
					if (it != tempVector.end()) {
						tempVector.push_back(filename);
						fileTable[stem] = tempVector;
					}
				}
				else {
#ifdef _DEBUG
					std::cout << "        Stem does not exist in the file table" << std::endl;
#endif
					//stem does not exist in the file table so insert
					std::vector<std::string> tempFileList;
					tempFileList.clear();
					tempFileList.push_back(filename);
					fileTable.insert({ stem, tempFileList });
				}


				files.push_back(de.path().string());

			}
		}

		
		//iterate through the filetable and compile each entry
		std::map<std::string, std::vector<std::string>>::iterator it;
		//loop through the map
		//man I really hate how ugly iterators are....
		for (it = fileTable.begin(); it != fileTable.end(); ++it) {
			std::vector<std::string> fileList = it->second;
			FileProcessor fp;
			//fp.flags = this->flags;
			fp.DoCommandFiles(fileList);
			//this->DoCommandFiles(fileList);
		}

		if (this->flags.HasFlag(FlagType::FT_RECURSE)) {
			for (std::string dir : directories) {
				//this->ProcessDirectory(dir);
			}
		}
		//then check if flags.HasFlag(FlagType::FT_RECURSE) and recurse into the lower directories
	}
	else {
		std::cout << "Warning: Directory path " << path << " does not exist!" << std::endl;
	}

}

//Command processor for the dir command
void FileProcessor::DoCommandDir(std::vector<std::string> args) {
#ifdef _DEBUG
	std::cout << "Directory processing is not yet implemented!\n";
#endif
	/*bool recurse = flags.HasFlag(FlagType::FT_RECURSE);

	for (std::string path : args) {
		ProcessDirectory(path);
	}
	*/
}

//Command processor for the file command
void FileProcessor::DoCommandFile(std::vector<std::string> args) {
	//some global variables
	std::string typeOverride = "";

	std::string outputOverride = "";

	std::string includeDir = "";

	bool noPack = false;



	//First thing we need to do is check out the flags and see if they work for this command

	//check for a type override flag
	if (flags.HasFlag(FlagType::FT_TYPE)) {

		//get the value of the Type flag
		typeOverride = std::string(flags.GetFlag(FlagType::FT_TYPE).text);

		//If the size is 1 then its the one character code for the type
		if (typeOverride.size() == 1) {

			//This also converts the type override into the format that glslangvalidator expects
			switch (typeOverride[0]) {
			case 'v': typeOverride = "-S vert";
				break;
			case 'g': typeOverride = "-S geom";
				break;
			case 'f': typeOverride = "-S frag";
				break;
			case 'e': typeOverride = "-S tese";
				break;
			case 'c': typeOverride = "-S tesc";
				break;
			case 'u': typeOverride = "-S comp";
				break;
			default: {
				throw std::runtime_error("Invalid shader type specified in type override flag: " + typeOverride + "\n");
			}
			}
		}
		//Otherwise we have to check for individual type words
		else {
			if (typeOverride == "vert" || typeOverride == "geom" || typeOverride == "frag" ||
				typeOverride == "tese" || typeOverride == "tesc" || typeOverride == "comp") {
				//Append the -S so we can apply directly to the command line
				typeOverride = "-S " + typeOverride;
			}
			else {
				//If we didnt encounter any of the valid ones throw an error
				throw std::runtime_error("Invalid shader type specified in type override flag: " + typeOverride + "\n");
			}
		}
	}

	//check for an output override flag
	if (flags.HasFlag(FlagType::FT_OUTPUT)) {
		//get the value of the Type flag
		//We dont have to do any other than prepend the -o flag because its a file name.
		outputOverride = std::string(flags.GetFlag(FlagType::FT_OUTPUT).text);
		outputOverride = "-o " + outputOverride;
	}

	//check for include directory override flag
	if (flags.HasFlag(FlagType::FT_INCLUDE)) {

		//Again not checking this for anything so good luck end user!
		includeDir = std::string(flags.GetFlag(FlagType::FT_INCLUDE).text);
		includeDir = "-I " + includeDir;
	}

	//check for nopack flag
	if (flags.HasFlag(FlagType::FT_NOPACK)) {
		noPack = true;
	}


	//If noPack is not set then we need to open the raw file and collect the variable and descriptor sets
	if (!noPack) {
		//pack the ts file
	}


	//Building this specifically for vulkan right now so going to hardcode in the -V
	//But later on it would be cool to add a flag for vulkan or opengl
	std::string command = "glslangvalidator.exe -V " + args[0] + " " + typeOverride + " " + outputOverride + " " + includeDir;

	//std::cout << "Trying to run the command glslangvalidator.exe " + command + "\n";
	//ShellExecuteA(NULL, "open", "glslangvalidator.exe", command.c_str(), 0, SW_SHOW);


	//This command spawns a child process that runs the given command
	//We can monitor the output through the returned handle
	//HANDLE cmdHandle = SpawnProcess(command);
	struct ProcessInfo info = SpawnProcess(command);


	//Set up a buffer to read in data from the spawned process
	DWORD readBytes = 0;
	char buff[1024];


	//I really hate having a while(true) but it seems to be the best way to do it
	while (true) {

		//We need to clear out the buffer every loop otherwise we'll get some duplicate data
		memset(buff, 0, 1024);
		BOOL readOk = FALSE;

		//Try to read in some data from the handle
		readOk = ReadFile(info.OutputHandle, buff, 1024, &readBytes, NULL);
		//when the read fails or we have no bytes left we can exit
		if (!readOk || readBytes == 0) break;

		//Print the read bytes to the screen
		std::cout << buff;
	}

	WaitForSingleObject(info.pInfo.hProcess, INFINITE);
	DWORD exit_value;

	GetExitCodeProcess(info.pInfo.hProcess, &exit_value);

	std::cout << "Process exited with code: " << exit_value << "\n";

	CloseHandle(info.pInfo.hProcess);
	CloseHandle(info.pInfo.hThread);
	//Then don't forget to close the handle
	CloseHandle(info.OutputHandle);

}

//Command processor for the files command
void FileProcessor::DoCommandFiles(std::vector<std::string> args) {
	std::string typeOverride = "";

	std::string outputOverride = "";

	std::string includeDir = "";

	bool noPack = false;


	if (flags.HasFlag(FlagType::FT_OUTPUT)) {
		//get the value of the Type flag
		//We dont have to do any other than prepend the -o flag because its a file name.
		outputOverride = std::string(flags.GetFlag(FlagType::FT_OUTPUT).text);
		outputOverride = outputOverride;
	}


	//check for include directory override flag
	if (flags.HasFlag(FlagType::FT_INCLUDE)) {

		//Again not checking this for anything so good luck end user!
		includeDir = std::string(flags.GetFlag(FlagType::FT_INCLUDE).text);
		includeDir = "-I " + includeDir;
	}

	//check for nopack flag
	if (flags.HasFlag(FlagType::FT_NOPACK)) {
		noPack = true;
	}

	int fileCount = args.size();

	std::string vertStage, tescStage, teseStage, geomStage, fragStage, compStage;
	for (int i = 0; i < fileCount; ++i) {
		std::string extension = args[i].substr(args[i].find_last_of(".") + 1);
		if (extension == "vert") {
			vertStage = args[i];
		}
		else if (extension == "tesc") {
			tescStage = args[i];
		}
		else if (extension == "tese") {
			teseStage = args[i];
		}
		else if (extension == "geom") {
			geomStage = args[i];
		}
		else if (extension == "frag") {
			fragStage = args[i];
		}
		else if (extension == "comp") {
			compStage = args[i];
		}
	}


	std::string* commands = new std::string[fileCount];
	struct ProcessInfo* infos = new ProcessInfo[fileCount];

	for (int i = 0; i < fileCount; ++i) {
		std::string extension = args[i].substr(args[i].find_last_of(".") + 1);
		std::string glsloutput = "";
		if (outputOverride != "") {
			glsloutput = "-o " + outputOverride + "." + extension + ".spv";
		}

		commands[i] = "glslangvalidator.exe -V " + args[i] + " " + glsloutput + " " + includeDir;
#ifdef _DEBUG
		std::cout << "Spawning process with command: " << commands[i] << std::endl;
#endif
		infos[i] = SpawnProcess(commands[i]);
	}
	//Building this specifically for vulkan right now so going to hardcode in the -V
	//But later on it would be cool to add a flag for vulkan or opengl
	//std::string command = "glslangvalidator.exe -V " + args[0] + " " + outputOverride;

	//std::cout << "Trying to run the command glslangvalidator.exe " + command + "\n";
	//ShellExecuteA(NULL, "open", "glslangvalidator.exe", command.c_str(), 0, SW_SHOW);


	//This command spawns a child process that runs the given command
	//We can monitor the output through the returned handle
	//HANDLE cmdHandle = SpawnProcess(command);
	//struct ProcessInfo info = SpawnProcess(command);


	//Set up a buffer to read in data from the spawned process
	DWORD readBytes = 0;
	char buff[1024];


	for (int i = 0; i < fileCount; ++i) {
		readBytes = 0;
		while (true) {
			memset(buff, 0, 1024);
			BOOL readOK = ReadFile(infos[i].OutputHandle, buff, 1024, &readBytes, NULL);
			if (!readOK || readBytes == 0) break;
			std::cout << buff;
		}
		WaitForSingleObject(infos[i].pInfo.hProcess, INFINITE);
		DWORD exit_value;

		GetExitCodeProcess(infos[i].pInfo.hProcess, &exit_value);
		//std::wcout << "Process " << i << " exited with code " << exit_value << "\n";





		CloseHandle(infos[i].pInfo.hProcess);
		CloseHandle(infos[i].pInfo.hThread);
		CloseHandle(infos[i].OutputHandle);
	}


	if (!noPack) {
		//pack up the file

		//Get the stages



		ShaderPack pack;
		std::vector<Layout> vertLayout, tescLayout, teseLayout, geomLayout, fragLayout, compLayout;
		vertLayout.clear();
		tescLayout.clear();
		teseLayout.clear();
		geomLayout.clear();
		fragLayout.clear();
		compLayout.clear();


		if (vertStage != "") {

			vertLayout = pack.ParseGLSL(vertStage);

			std::ifstream instream(outputOverride == "" ? "vert.spv" : outputOverride + ".vert.spv", std::ios::binary | std::ios::ate);
			if (!instream.is_open()) {
				std::cout << "Failed to open file for deserialization\n";
				return;
			}
			uint64_t size = instream.tellg();
			unsigned char* code = new unsigned char[size];

			instream.seekg(0, std::ios::beg);
			uint64_t readBytes = 0;
			instream.read((char*)code, size);
			instream.close();


			pack.AddStage(code, size, vertLayout, LayoutStage::LAYOUT_VERTEX);

		}
		if (tescStage != "") {

			tescLayout = pack.ParseGLSL(tescStage);

			std::ifstream instream(outputOverride == "" ? "tesc.spv" : outputOverride + ".tesc.spv", std::ios::binary | std::ios::ate);
			if (!instream.is_open()) {
				std::cout << "Failed to open file for deserialization\n";
				return;
			}
			uint64_t size = instream.tellg();
			unsigned char* code = new unsigned char[size];

			instream.seekg(0, std::ios::beg);
			uint64_t readBytes = 0;
			instream.read((char*)code, size);
			instream.close();

			pack.AddStage(code, size, tescLayout, LayoutStage::LAYOUT_TESSELLATION_CONTROL);
		}
		if (teseStage != "") {
			teseLayout = pack.ParseGLSL(teseStage);

			std::ifstream instream(outputOverride == "" ? "tese.spv" : outputOverride + ".tese.spv", std::ios::binary | std::ios::ate);
			if (!instream.is_open()) {
				std::cout << "Failed to open file for deserialization\n";
				return;
			}
			uint64_t size = instream.tellg();
			unsigned char* code = new unsigned char[size];

			instream.seekg(0, std::ios::beg);
			uint64_t readBytes = 0;
			instream.read((char*)code, size);
			instream.close();

			pack.AddStage(code, size, teseLayout, LayoutStage::LAYOUT_TESSELLATION_EVALUATION);

		}
		if (geomStage != "") {
			geomLayout = pack.ParseGLSL(geomStage);

			std::ifstream instream(outputOverride == "" ? "geom.spv" : outputOverride + ".geom.spv", std::ios::binary | std::ios::ate);
			if (!instream.is_open()) {
				std::cout << "Failed to open file for deserialization\n";
				return;
			}
			uint64_t size = instream.tellg();
			unsigned char* code = new unsigned char[size];

			instream.seekg(0, std::ios::beg);
			uint64_t readBytes = 0;
			instream.read((char*)code, size);
			instream.close();

			pack.AddStage(code, size, geomLayout, LayoutStage::LAYOUT_GEOMETRY);

		}
		if (fragStage != "") {
			fragLayout = pack.ParseGLSL(fragStage);

			std::ifstream instream(outputOverride == "" ? "frag.spv" : outputOverride + ".frag.spv", std::ios::binary | std::ios::ate);
			if (!instream.is_open()) {
				std::cout << "Failed to open file for deserialization\n";
				return;
			}
			uint64_t size = instream.tellg();
			unsigned char* code = new unsigned char[size];

			instream.seekg(0, std::ios::beg);
			uint64_t readBytes = 0;
			instream.read((char*)code, size);
			instream.close();

			pack.AddStage(code, size, fragLayout, LayoutStage::LAYOUT_FRAGMENT);
		}
		if (compStage != "") {
			compLayout = pack.ParseGLSL(compStage);

			std::ifstream instream(outputOverride == "" ? "comp.spv" : outputOverride + ".comp.spv", std::ios::binary | std::ios::ate);
			if (!instream.is_open()) {
				std::cout << "Failed to open file for deserialization\n";
				return;
			}
			uint64_t size = instream.tellg();
			unsigned char* code = new unsigned char[size];

			instream.seekg(0, std::ios::beg);
			uint64_t readBytes = 0;
			instream.read((char*)code, size);
			instream.close();

			pack.AddStage(code, size, fragLayout, LayoutStage::LAYOUT_COMPUTE);
		}



		if (outputOverride != "") {
			pack.Serialize(outputOverride + ".sp");
		}
		else
		{
			pack.Serialize("ShaderPack.sp");
		}

	}
}

//A function to spawn a windows process and run the given command
//Returns a handle to the spawned process's std out pipe for reading
//HANDLE SpawnProcess(std::string command) {
struct ProcessInfo FileProcessor::SpawnProcess(std::string command) {
	//BEGIN WINDOWS PROGRAMMING FUCKERY

	//Working with MSDN article: https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN
	SECURITY_ATTRIBUTES cmdSAttr;
	cmdSAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	cmdSAttr.bInheritHandle = TRUE;
	cmdSAttr.lpSecurityDescriptor = NULL;

	HANDLE cmdSTD_IN_RD = NULL;
	HANDLE cmdSTD_IN_WR = NULL;
	HANDLE cmdSTD_OUT_RD = NULL;
	HANDLE cmdSTD_OUT_WR = NULL;


	//Create the pipes and set up the handles
	if (!CreatePipe(&cmdSTD_OUT_RD, &cmdSTD_OUT_WR, &cmdSAttr, 0)) {
		throw(std::runtime_error("Failed to create the child standard out pipe\n"));
	}

	if (!SetHandleInformation(cmdSTD_OUT_RD, HANDLE_FLAG_INHERIT, 0)) {
		throw(std::runtime_error("Failed to set child handle information std out\n"));
	}

	//this probably links these two together as well for writing to the child process
	if (!CreatePipe(&cmdSTD_IN_RD, &cmdSTD_IN_WR, &cmdSAttr, 0)) {
		throw(std::runtime_error("Failed to create the child standard in pipe\n"));
	}

	if (!SetHandleInformation(cmdSTD_IN_WR, HANDLE_FLAG_INHERIT, 0)) {
		throw(std::runtime_error("Failed to set child handle information std in\n"));
	}

	PROCESS_INFORMATION cmdProcInfo;
	STARTUPINFO cmdStartInfo;

	//Clear out the memory for the process info and startup info
	ZeroMemory(&cmdProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&cmdStartInfo, sizeof(STARTUPINFO));


	//Set up the standard IO pipe redirection
	cmdStartInfo.cb = sizeof(STARTUPINFO);
	cmdStartInfo.hStdError = cmdSTD_OUT_WR;
	cmdStartInfo.hStdOutput = cmdSTD_OUT_WR;
	cmdStartInfo.hStdInput = cmdSTD_IN_RD;
	cmdStartInfo.dwFlags |= STARTF_USESTDHANDLES;


	//CreateProcess requires a TCHAR* for the command input, mbstowcs_s takes care of conversion
	TCHAR commandLine[255] = { 0 };
	mbstowcs_s(nullptr, commandLine, command.c_str(), command.length());

	if (!CreateProcess(NULL,
		commandLine,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&cmdStartInfo,
		&cmdProcInfo)) {
		std::string error_message = "Failed to create child process! Error code: " + std::to_string(GetLastError());
		throw(std::runtime_error(error_message));
	}

	//Then close the handles we dont need
	//I guess its bad practice to not close handles? Never thought I'd be a windows programmer
	//CloseHandle(cmdProcInfo.hProcess);
	//CloseHandle(cmdProcInfo.hThread);

	//Ok so cmdProcInfo.hProcess can be used to get the return code once the process has finished.
	//https://stackoverflow.com/questions/9369823/how-to-get-a-sub-process-return-code

	//Something like: WaitForSingleObject(cmdProcInfo.hProcess, INFINITE);//wait for process to exit or become single thread again
	//DWORD exit_code;
	//GetExitCodeProcess(cmdProcInfo.hProcess, &exit_code);

	CloseHandle(cmdSTD_OUT_WR);
	CloseHandle(cmdSTD_IN_RD);
	CloseHandle(cmdSTD_IN_WR);

	//Then return the handle needed to read the output of the program
	//return cmdSTD_OUT_RD;

	ProcessInfo p = { cmdSTD_OUT_RD, cmdProcInfo };
	return p;
}