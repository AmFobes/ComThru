#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring> // strerror()
#include <unistd.h> // fork()
#include <sys/wait.h>
#include <vector>
#include <sstream>


bool fExists(std::string filename){
		std::ifstream ifile(filename);
	return (bool)ifile;
}

int main(int argc, char *argv[]){
	std::cout << "ComThru, received " << argc - 1 << " arguments\n";
	
	/*
	 *
	 * Check program arguments
	 *
	 */
	
	if(argc <= 1){
invalidArgs:
		std::cout << "Invalid number of arguments / invalid arguments, usage:\n" << argv[0] << " [filename.ext] [-b (binary mode)\n";
		return 0;
	}
	
	bool binaryMode = false;
	std::string file;
	if(argc > 2){
		if(strcmp(argv[1],"-b") == 0){
			binaryMode = true;
			std::cout << "Using binary mode\n";
			file = argv[2];
		}
		else if(strcmp(argv[2],"-b") == 0){
			binaryMode = true;
			std::cout << "Using binary mode\n";
			file = argv[1];
		}
		else{
			goto invalidArgs; // Oh no goto 
		}
	}
	else{
		file = argv[1];
	}
	
	/*
	 *
	 * Check for configuration file presence
	 *
	 */

   	struct stat fileStats;
	if(stat("ComThru.cfg",&fileStats) == -1){
		/* Unable to open file*/
		std::cout << "Unable to open configuration file\n"
			<< "errno:" << errno
			<< "\nstrerror():" << strerror(errno) << std::endl;
		return 0;
	}
	
	/*
	 *
	 * Read configuration
	 *
	 */

	std::string port;
	unsigned int baudRate;

	std::ifstream configFile("ComThru.cfg");

	if(configFile.is_open()){
		std::string line;
		while(getline(configFile,line)){
			if(line[0] == '#' || line.empty())
		     	continue;
            
			auto delimiterPos = line.find("=");
			auto name = line.substr(0, delimiterPos);
			auto value = line.substr(delimiterPos + 1);
			
			if(name == "port")
				port = value;
			
			if(name == "baudrate"){
				try{
					baudRate = std::stoul(value,nullptr);
				}
				catch(std::invalid_argument){
					std::cout << "The baud rate in the configuration is invalid\n";
					return 0;
				}
				catch(std::out_of_range){
					std::cout << "The baud rate in the configuration is too large\n";
					return 0;
				}
			}
		}
	}

	/*
	 *
	 * Check if ckermit is avaliable
	 *
	 */	
	
	std::string kermitFileName;

	if(fExists("wermit"))
		kermitFileName = "wermit";
	else if (fExists("kermit"))
		kermitFileName = "kermit";
	else if (fExists("ckermit"))
		kermitFileName = "ckermit";
	else{
		std::cout << "Unable to find c-kermit, valid filenames (case sensitive)\nwermit\nkermit\nckermit\n";
		return 0;
	}


	std::cout << "Preparing to fork and run child process \n";
	
	int status;
	pid_t wermit_pid = fork();

	if(wermit_pid >= 0){
			if(wermit_pid == 0){
				std::string commandBase;
				if(binaryMode)	
				commandBase = "set carrier-watch off,set file type binary,send ";
				else
				commandBase = "set carrier-watch off,send ";
				// carrier-watch off because my serial device doesn't support it
			    char* arr[] = 
				{
				strdup(kermitFileName.c_str()), 			// Required if ckermit uses argv[0]
				"-m", 										// set modem type
				"none", 									// modem type none
				"-l", 										// set serial device
				strdup(port.c_str()), 						// serial device 
				"-b", 										// set baud rate
				strdup(std::to_string(baudRate).c_str()),	// baud rate
				"-C",										// interactive mode; custom command
				strdup(std::string(commandBase + file + ",quit").c_str()), // the custom commands required
				nullptr, // Always end with nullptr when passing args into execv
				};
				
				auto result = execv(kermitFileName.c_str(),arr);
				return result;
			}
			else{
				std::cout << "Child process has pid of " << wermit_pid << std::endl;
				do{
						waitpid(wermit_pid,&status,WUNTRACED);
						
				}
				while(!WIFEXITED(status));
				std::cout << strerror(errno) << std::endl;
				std::cout << "Child process exited with code" << WEXITSTATUS(status) << std::endl;
				return 0;
			}
	}
	else{
			std::cout << "Fork failed\n"
					<< "errno: " << errno
					<< "\nstrerror(): " << strerror(errno) << std::endl;
	}
	return 0;
}
