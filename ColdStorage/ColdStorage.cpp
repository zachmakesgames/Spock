//ColdStorage.cpp : A tool for packing GLSL shaders into portable and easy to use ShaderPacks.
//Takes raw shader files and produces a file that can be loaded directly into a program that uses ShaderPacks.


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>


//Stuff for creating the child processes
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#include "ShaderPack_Public.h"
#include "FileProcessing.h"


//Usage Syntax info below

//Usage syntax:
//ColdStorage.exe [command] [arguments] [optional flags]
//Possible commands:
//          dir:    Compiles all shaders in a given directory
//                  Optional: The flag -t may be used to specify only specific types of shaders to compile
//                            within the given directory.
//                            Possible types: (v)ertex, (g)eometry, (f)ragment, tessellation_(e)val, tessellation_(c)ontrol, comp(u)te
//                            Type flags can be used in any combination and any order so long as the specified characters are used.
//                            If only one type is desired the whole word type can be used instead of the specified character
//                            Examples: ColdStorage.exe dir /path/to/shaders -t vgfecu
//                                      ColdStorage.exe dir /path/to/shaders -t vertex
//                                      ColdStorage.exe dir /path/to/shaders -t tessellation_eval
//                                      ColdStorage.exe dir /path/to/shaders -t vgf
//                          
//                          The flag -o may be used to specify a postfix to the output file names. 
//                          Examples: ColdStorage.exe dir /path/to/shaders -t vertex -o _ExamplePostfix
//                                    Example inputs would be: shaderfile1.vert, myVertexShader.vert
//                                    Example outputs would be: shaderfile1_ExamplePostfix.sp, myVertexShader_ExamplePostfix.sp
//
//          file:   Compiles the specified file
//                  Optional: The flag -t may be used to override the file type
//                            Possible types: (v)ertex, (g)eometry, (f)ragment, tessellation_(e)val, tessellation_(c)ontrol, comp(u)te
//                            Examples: ColdStorage.exe file myshader.vert
//                                      ColdStorage.exe file myshader.file -t vertex
//                                      ColdStorage.exe file myshader.file -t f
//
//          files:  Compiles multiple files into a single .sp pack file
//                  Optional: The flag -t may be used to override the file types and order. However if this flag is used the number of
//							  types specified MUST match the number of files given and no two types may be re-used
//                            Possible types: (v)ertex, (g)eometry, (f)ragment, tessellation_(e)val, tessellation_(c)ontrol, comp(u)te
//                            Examples: ColdStorage.exe files myshader.vert myshader.frag
//                                      ColdStorage.exe files vertexshader.vert myshader.geom file1.frag
//                                      ColdStorage.exe files myshader1.file myshader2.file -t vf		
//
//
//		If no command or arguments are given then ColdStorage will try to compile all shaders of all types in the current running directory.
//
//      Possible Flags:
//                  -t:                 Generally specifies type, see individual command for specific useage
//                  -o:                 Overrides output file name with name that follows. Special use case with the dir command.
//                  -n,-nopack:         Overrides the packing function and just compiles shaders producing SPIRV files instead of .sp pack files.
//                  -v:                 Verifies shader files but does not compile to SPIRV or pack to .sp packs.
//					-threads:			Specifies the number of threads to try and use to parallelize compilation.
//					-d:					Specifies the directory that the SPIRV compiler should look in for #includes





//GLSLangValidator default type extensions:
//.vert - a vertex shader
//.tesc - a tessellation control shader
//.tese - a tessellation evaluation shader
//.geom - a geometry shader
//.frag - a fragment shader
//.comp - a compute shader

//And subsequent -S stage options: vert, tesc, tese, geom, frag, or comp
//le duh


///Typedefs, structs, enums oh my!
typedef struct Command {
	char name[32];
	bool requiresArguments;
	int requiredArgumentCount;		//Special magic trick: If requiredArgumentCount is positive then it is a hard match meaning it must be exactly equal to
									//If requiredArgumentCount is negative then it is a minimum value, meaning it requires >= requiredArgumentCount args
}Command;

FileProcessor processor;
/*typedef struct ProcessInfo {
	HANDLE OutputHandle;
	PROCESS_INFORMATION pInfo;
};*/


///Global variables
//Flags flags;

//A cheap and easy way to register commands
Command validCommands[] = { 
	{"dir", true, 1}, 
	{"file", true, 1},
	{"files", true, -1}			//See the note above about negative numbers being a lower bound instead of a hard match
};


///Function Prototypes
bool IsValidCommand(char* command, int* commandIndex = nullptr);
bool CommandRequiresArguments(int commandIndex);
int CommandArgumentCount(int commandIndex);

bool IsValidCommandInput(int count, char** arguments);

void HandleCommand(int count, char** arguments);


bool IsFlag(char* testFlag, FlagType* outType);
char** GetFlags(int count, char** arguments);


/*void ProcessDirectory(std::string path);
void DoCommandDir(std::vector<std::string> args);
void DoCommandFile(std::vector<std::string> args);
void DoCommandFiles(std::vector<std::string> args);

//HANDLE SpawnProcess(std::string command);
struct ProcessInfo SpawnProcess(std::string command);
*/

void PrintSyntax();





///The main function, the entry point for the program
int main(int argc, char** argv)
{
	std::cout << "ColdStorage bulk shader packing tool.\n";
#ifdef _DEBUG
	std::cout << "There are " << sizeof(validCommands)/sizeof(Command) << " commands registered\n";
#endif



	//TestPack();

	
	//Temporarily disabling the main functionality to test pack interpretation
	char** arguments = nullptr;
	//Getting the flags should be done in a try/catch because it throws errors for invalid syntax
	try {
		arguments = GetFlags(argc, argv);
	}
	catch (std::runtime_error e) {
		//If we get a runtime_error we can assume its from one of our throws
		//Then print the useage syntax and exit with a negative error code
		std::cout << "Error: " << e.what() << "\n";
		PrintSyntax();
		return -1;
	}
	std::cout << "Called with " << processor.flags.Count() << " flags\n";

	//Make sure that arguments is not null because we need it later on
	if (arguments == nullptr) {
		std::cout << "Something went very wrong getting flags!\n";
		return -1;
	}
	
	//now get the number of arguments in the list
	//NOTE: GetFlags strips the program name out of the arguments list!
	int argCount = 0;
	for (int i = 0; i < argc; ++i) {
		if (arguments[i] != nullptr) {
			++argCount;
		}
		else {
			break;
		}
	}
	

	//now we need to verify that the command is a valid command and has the proper input it needs
	if (!IsValidCommandInput(argCount, arguments)) {
		//IsValidCommand should print the appropriate error for us so we should just exit if
		//it returns false
		return -1;
	}

	//Now that we have a valid command and all our flags
	
	
	///
	/// BEGIN COMMAND PROCESSING
	///

	try {

		HandleCommand(argCount, arguments);
	}
	catch (std::runtime_error e) {
		std::cout << "Error: " << e.what() << "\n";
		PrintSyntax();
		return -1;
	}


	delete[] arguments;	//we dont need to free each individual element of arguments because they are shallow copied pointers from argv
	
	
	
}

//A simple function to determine which process handler should be run
//and to issue it. Packs up the parametes for the command nicely so we can stop
//passing around shitloads of pointers
void HandleCommand(int argCount, char** arguments) {

	//by now we've validated the shit out of the input so this should be safe
	std::string command = std::string(arguments[0]);
	std::vector<std::string> args;

	//Break up the arguments double array into an easier to swallow, one piece package to send to the command handlers
	for (int i = 1; i < argCount; ++i) {
		args.push_back(std::string(arguments[i]));
	}

	if (command == "dir") {
		processor.DoCommandDir(args);
	}

	if (command == "file") {
		processor.DoCommandFile(args);
	}

	if (command == "files"){
		processor.DoCommandFiles(args);
	}
}


//A function to validate the input of a command
//Returns true if the command and its list arguments are valid, false if not
//Note that this functions exepcts that arguments[0] is the actual command to validate and not the program name
//or anything else
bool IsValidCommandInput(int count, char** arguments) {

	//First make sure the count is greater than 0 so we dont waste our time
	if (count > 0) {

		//Then check if its in the list of valid commands and if so get its index
		int commandIndex = -1;
		if (IsValidCommand(arguments[0], &commandIndex)) {
			//If is valid then check if it requires additional arguments
			if (CommandRequiresArguments(commandIndex)) {
				//So check if we have enough arguments
				int requiredArgs = CommandArgumentCount(commandIndex);
				bool lowerBound = requiredArgs < 0;		//check if the requirement is a lower bound or a hard match
				if (lowerBound) requiredArgs *= -1;		//if its a lower bound then realign it to a positive number to process

				//First check for commands that have a hard requirement for arguments
				if ((count != (requiredArgs + 1)) && (lowerBound == false)) {
					//if argc is not equal to 1(the initial command) + requiredArgs then we dont have enough args
					//Show an error, print the syntax, and return -1
					std::cout << "Command " << validCommands[commandIndex].name << " requires exactly " << requiredArgs << " arguments but " << count - 1 << " were given.\n";
					PrintSyntax();
					return false;
				}
				else {
					if (count < requiredArgs + 1 & lowerBound) {
						//if argc is less than 2(the program name + the initial command) + requiredArgs then we dont have enough args
						//Show an error, print the syntax, and return -1
						std::cout << "Command " << validCommands[commandIndex].name << " requires at least " << requiredArgs << " arguments but " << count - 1 << " were given.\n";
						PrintSyntax();
						return false;
					}
					//Dont need an else because we can just start processing
				}
			}
		}
		else {
			//Otherwise print an error, show the syntax, and exit
			std::cout << "Error: Unknown command: " << std::string(arguments[0]) << "\n";
			PrintSyntax();
			return false;
		}

		return true;
	}
	else {	//If count <= 0 just return false
		return false;
	}
}

//A function to determine if a command is valid
//Note: commandIndex is prototyped to default to nullptr
bool IsValidCommand(char* command, int* commandIndex) {
	//Pretty straight forward, walk through the list of commands and look for one that matches
	for (int i = 0; i < sizeof(validCommands) / sizeof(Command); ++i) {
		if (std::string(command) == std::string(validCommands[i].name)) {

			//If optional argument commandIndex is not null, set it to the index we just found
			if (commandIndex != nullptr) {
				*commandIndex = i;
			}
			return true;
		}
	}
	return false;
}

//A function to determine if a command requires additional arguments
//Expects that commandIndex is a valid index in the list of validCommands
//commandIndex should be retreived with IsValidCommand before calling this function
//Note: if commandIndex is out of range returns false
bool CommandRequiresArguments(int commandIndex) {
	if (commandIndex < 0 || commandIndex >= sizeof(validCommands) / sizeof(Command)) {
		return false;
	}
	else {
		return validCommands[commandIndex].requiresArguments;
	}
}


//A function to get the number of required arguments for a command
//Expects that commandIndex is a valid index in the list of validCommands
//commandIndex should be retreived with IsValidCommand before calling this function
//Note: if commandIndex is out of range returns -1
int CommandArgumentCount(int commandIndex) {
	if (commandIndex < 0 || commandIndex >= sizeof(validCommands) / sizeof(Command)) {
		return -1;
	}
	else {
		return validCommands[commandIndex].requiredArgumentCount;
	}
}

//A function to check if a string is a flag
bool IsFlag(char* testFlag, FlagType* outType) {
	int testFlagLen = strlen(testFlag);
	if (testFlagLen > 0 && testFlag[0] == '-') {

		//Now make sure it has enough characters to be a valid argument
		if (testFlagLen > 1) {
			switch (testFlag[1]) {
			case 't': {	//special case here because the type and threads arguments both start with t
				if (!strcmp(testFlag, "-t")) {	//check for just -t, the type argument
					if(outType != nullptr) *outType = FlagType::FT_TYPE;		//Last minute added a safety net if
				}
				else {
					if (!strcmp(testFlag, "-threads") || !strcmp(testFlag, "-thread")) {
						if (outType != nullptr)  *outType = FlagType::FT_THREAD;
					}
					else {
						return false;
					}
				}
			}
				break;
			case 'o': if (outType != nullptr)  *outType = FlagType::FT_OUTPUT;
				break;
			case 'n': if (outType != nullptr)  *outType = FlagType::FT_NOPACK;
				break;
			case 'v': if (outType != nullptr)  *outType = FlagType::FT_VERIFY;
				break;
			case 'd': if (outType != nullptr) *outType = FlagType::FT_INCLUDE;
				break;
			case 'r': if (outType != nullptr) *outType = FlagType::FT_RECURSE;
				break;
			default: return false;	//Don't need a break because it would never be reached
			}
		}

		//We can return true here because all error conditions would have already returned false
		return true;

	}
	else {	//Almost forgot to return false for any other case, whoops
		return false;
	}
}

//A function to take in the arguments to main and pick out the flags from the argument list
//Remember that argv[0] is the name of the program!
//Returns a list containing in order all arguments that were not part of a flag
//NOTE: The arguments list returned does NOT contain the initial argv[0] of the program name
char** GetFlags(int count, char** arguments) {

	//This check is here to avoid memcpy trying to copy 0 bytes
	if (count == 0) {
		return nullptr;
	}


	//Create the list of arguments that are not flags and clear it out
	//It's safe to use the same count because theres not way we could
	//end up with more arguments than we started with, and we can easily
	//get the count by looking for the first null terminator
	char** notFlags = new char* [count];
	//Using a for loop to null it out because memcpy was not happy
	//about the possibililty of something being 0 or some other bullshit
	for (int i = 0; i < count; ++i) notFlags[i] = nullptr;

	int notFlagsCount = 0;	//used for adding new items to the list


	//Crawl through the argument array and look for flags that start with '-'
	for (int i = 1; i < count; ++i) {
		FlagType testType = FlagType::EMPTY;

		//Check if it is a flag and if so get the type
		if (IsFlag(arguments[i], &testType)) {
			//If it is a flag then we need to verify that if it should have an argument that the argument is next in line
			
			//These types do not require arguments and can be used on their own
			if (testType == FlagType::FT_NOPACK || testType == FlagType::FT_VERIFY) {
				Flag f = { testType, "" };
				processor.flags.AddFlag(f);		//We also dont need to check the output of this because we dont care about duplicate flags with no arguments
			}

			//All other types require a following argument so we need to verify that the next argument exists
			else {
				//First check if there even is a next index
				int nextIndex = i + 1;
				if (i + 1 >= count) {
					//there is no next index so lets throw an error
					throw std::runtime_error("Invalid Syntax: Flag " + std::string(arguments[i]) + " requires additional arguments\n");
				} 
				else{
					//If there is a next index lets make sure that the string wouldn't overflow the text field of Flag
					int argumentLen = strlen(arguments[nextIndex]);
					if (argumentLen > 255) {	//Note: Flag.text is of sie 256, but we need to ensure space for the null terminator, so 255 to be safe
						throw std::runtime_error("Argument to flag " + std::string(arguments[i]) + " exceeds the maximum length of 255 characters\n");
					}
					else {
						//Otherwise create the flag and add it to the list
						Flag f;
						f.t = testType;
						strcpy_s(f.text, arguments[nextIndex]);

						if (!processor.flags.AddFlag(f)){
							//We want to actually check this here because this may be a user error.
							//We wont throw an runtime_error but we will display a warning to the user
							std::cout << "WARNING: Flag " << std::string(arguments[i]) << " was previously declared, is this an error?\n";

						}

						//Lastly we now want to skip assesment of the next index, so lets incriment i to skip the next element
						++i;

						//Skipping the next element is important because the argument to a flag may begin with '-' in the case
						//of the -o flag for the dir command and we dont want to try and interpret an argument as a flag anyways
					}
				}
			}
		}
		else {
			notFlags[notFlagsCount++] = arguments[i];
		}
	}

	return notFlags;
}



///
/// BEGIN COMMAND PROCESSORS
///

/*
void ProcessDirectory(std::string path) {
	std::vector<std::string> files;
	files.clear();

	std::vector<std::string> directories;
	directories.clear();


	if (std::filesystem::exists(path)) {
		//if the path exists, get all files in the directory and organize by name and extension
		//files with the same prefix will be grouped together and compiled together if possible
		for (std::filesystem::directory_entry de : std::filesystem::directory_iterator(path)) {
			if (de.is_directory) {
				directories.push_back(de.path);
			}
			else {
				files.push_back(de.path);
				
			}
		}

		//arrange and process the files in this directory

		//then check if flags.HasFlag(FlagType::FT_RECURSE) and recurse into the lower directories
	}
	else {
		std::cout << "Warning: Directory path " << path << " does not exist!" << std::endl;
	}

}

//Command processor for the dir command
void DoCommandDir(std::vector<std::string> args) {
#ifdef DEBUG
	std::cout << "Directory processing is not yet implemented!\n";
#endif
	bool recurse = flags.HasFlag(FlagType::FT_RECURSE);

	for (std::string path : args) {
		if (std::filesystem::exists(path)) {
			//if the path exists, get all files in the directory and organize by name and extension
			//files with the same prefix will be grouped together and compiled together if possible
			for (std::filesystem::directory_entry d : std::filesystem::directory_iterator(path)) {
				
			}
		}
		else {
			std::cout << "Warning: Directory path " << path << " does not exist!" << std::endl;
		}
	}

}

//Command processor for the file command
void DoCommandFile(std::vector<std::string> args) {
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
void DoCommandFiles(std::vector<std::string> args) {
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
#ifdef DEBUG
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
struct ProcessInfo SpawnProcess(std::string command){
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
*/


///
/// END COMMAND PROCESSORS
///

//A function to print the usage syntax
//A nice way to be able to do this in multiple places without hating life
void PrintSyntax() {
std::cout << "Usage syntax:\n";
std::cout << "ColdStorage.exe [command] [arguments] [optional flags]\n";
std::cout << "Possible commands:\n";
std::cout << "          dir:    Compiles all shaders in a given directory\n";
std::cout << "                  Optional: The flag -t may be used to specify only specific types of shaders to compile\n";
std::cout << "                            within the given directory.\n";
std::cout << "                            Possible types: (v)ertex, (g)eometry, (f)ragment, tessellation_(e)val, tessellation_(c)ontrol, comp(u)te\n";
std::cout << "                            Type flags can be used in any combination and any order so long as the specified characters are used.\n";
std::cout << "                            If only one type is desired the whole word type can be used instead of the specified character\n";
std::cout << "                            Examples: ColdStorage.exe dir /path/to/shaders -t vgfecu\n";
std::cout << "                                      ColdStorage.exe dir /path/to/shaders -t vertex\n";
std::cout << "                                      ColdStorage.exe dir /path/to/shaders -t tessellation_eval\n";
std::cout << "                                      ColdStorage.exe dir /path/to/shaders -t vgf\n";
std::cout << "                          \n";
std::cout << "                          The flag -o may be used to specify a postfix to the output file names. \n";
std::cout << "                          Examples: ColdStorage.exe dir /path/to/shaders -t vertex -o _ExamplePostfix\n";
std::cout << "                                    Example inputs would be: shaderfile1.vert, myVertexShader.vert\n";
std::cout << "                                    Example outputs would be: shaderfile1_ExamplePostfix.sp, myVertexShader_ExamplePostfix.sp\n";
std::cout << "\n";
std::cout << "          file:   Compiles the specified file\n";
std::cout << "                  Optional: The flag -t may be used to override the file type\n";
std::cout << "                            Possible types: (v)ertex, (g)eometry, (f)ragment, tessellation_(e)val, tessellation_(c)ontrol, comp(u)te\n";
std::cout << "                            Examples: ColdStorage.exe file myshader.vert\n";
std::cout << "                                      ColdStorage.exe file myshader.file -t vertex\n";
std::cout << "                                      ColdStorage.exe file myshader.file -t f\n";
std::cout << "\n";
std::cout << "          files:  Compiles multiple files into a single .sp pack file\n";
std::cout << "                  Optional: The flag -t may be used to override the file types and order. However if this flag is used the number of\n";
std::cout << "							  types specified MUST match the number of files given and no two types may be re-used\n";
std::cout << "                            Possible types: (v)ertex, (g)eometry, (f)ragment, tessellation_(e)val, tessellation_(c)ontrol, comp(u)te\n";
std::cout << "                            Examples: ColdStorage.exe files myshader.vert myshader.frag\n";
std::cout << "                                      ColdStorage.exe files vertexshader.vert myshader.geom file1.frag\n";
std::cout << "                                      ColdStorage.exe files myshader1.file myshader2.file -t vf		\n";
std::cout << "\n";
std::cout << "		If no command or arguments are given then ColdStorage will try to compile all shaders of all types in the current running directory.\n";
std::cout << "\n";
std::cout << "      Possible Flags:\n";
std::cout << "                  -t:                 Generally specifies type, see individual command for specific useage\n";
std::cout << "                  -o:                 Overrides output file name with name that follows. Special use case with the dir command.\n";
std::cout << "                  -n,-nopack:         Overrides the packing function and just compiles shaders producing SPIRV files instead of .sp pack files.\n";
std::cout << "                  -v:                 Verifies shader files but does not compile to SPIRV or pack to .sp packs.\n";
std::cout << "                  -threads:			Specifies the number of threads to try and use to parallelize compilation.\n";
std::cout << "                  -d:					Specifies the directory that the SPIRV compiler should look in for #includes\n";
}

