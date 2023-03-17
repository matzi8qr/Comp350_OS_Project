// Written by Tyler Wormald, Sean Howe, and Timothy Morel
// library of shell commands for operating system

void command(char*);
int tryCommand(char*, char*);

int PRINT_STRING = 0;  // printString(char* bx);
int READ_STRING = 1;  // readString(char* bx);
int READ_SECTOR = 2;  // readSector(char* bx, int cx);
int READ_FILE = 3;  // readFile(char* bx, char* cx, int* dx)
int EXECUTE_PROGRAM = 4;  // executeProgram(char* bx);
int TERMINATE = 5;  // void terminate();
int WRITE_SECTOR = 6; //void writeSector(char* bx, int cx);
int DELETE_FILE = 7; // void deleteFile(char* bx, int* cx);
int WRITE_FILE = 8; // void writeFile(char* bx, char* cx, int dx);
int KILL = 9; // hahahah 7 ate 9. void kill(int bx);

int main(){

	char input[80];
	enableInterrupts();

	while (1){
		syscall(PRINT_STRING, "::>", 0, 0);
		syscall(READ_STRING, input, 0, 0);
		command(input);
	}
}

// This function will be checking input against our list of commands
void command(char* input){

	char bufferArr[13312];
	char* buffer = bufferArr;
	int bufferRoot = buffer;
	char line[80];
	int sectors;
	int i;
	int commandLength;
	for (i = 0; i < 13312; i++) buffer[i] = 0;

	// type <fileName>
	// Prints out the contents of file fileName to the terminal
	if (tryCommand("type", input)){
		commandLength = 5;
		syscall(READ_FILE, input + commandLength, buffer, &sectors);
		if (!sectors) syscall(PRINT_STRING, "File not found", 0, 0);
		else syscall(PRINT_STRING, buffer, 0, 0);
	}

	// exec <fileName>
	// Executes the program in file fileName
	else if (tryCommand("exec", input)){
		commandLength = 5;
		syscall(READ_FILE, input + commandLength, buffer, &sectors);
		if (!sectors) syscall(PRINT_STRING, "File not found", 0, 0);
		else syscall(EXECUTE_PROGRAM, input + commandLength, 0, 0);
	}

	// dir
	// Prints out all the files found in the directory sector
	else if (tryCommand("dir", input)){
		syscall(READ_SECTOR, buffer, 2, 0);
		bufferRoot = buffer;
		while (buffer < bufferRoot + 512) {  // Iterate through the directory sector
			if (!buffer[0]) {  // Continue case on deleted file
				buffer += 32;
				continue;
			}
			if (!buffer[1]) buffer[1] = ' ';  // Just replaces null terminators with spaces
			if (!buffer[2]) buffer[2] = ' ';
			if (!buffer[3]) buffer[3] = ' ';
			if (!buffer[4]) buffer[4] = ' ';
			if (!buffer[5]) buffer[5] = ' ';
			buffer[6] = '\n';  // Change to printString friendly buffer
			buffer[7] = '\r';
			buffer[8] = '\0';
			syscall(PRINT_STRING, buffer, 0, 0);
			buffer = buffer + 32;
		}
		buffer = bufferRoot;
	}

	// del <fileName>
	// Deletes the specified file fileName
	else if (tryCommand("del", input)){
		commandLength = 4;
		syscall(DELETE_FILE, input + commandLength, &sectors, 0);
		if (sectors) syscall(PRINT_STRING, "File successfully deleted", 0, 0);
		else syscall(PRINT_STRING, "File not found", 0, 0);
	}

	// copy <targetFile> <destinationFile>
	// Copies all the bites from targetFile to destinationFile
	else if (tryCommand("copy", input)){
		commandLength = 5;
		input += commandLength;
		syscall(READ_FILE, input, buffer, &sectors);
		if (sectors) {
			while(input[0] != ' ') input++;  // move cursor over to the space, then one more
			input++;
			syscall(WRITE_FILE, buffer, input, sectors);
		}
		else syscall(PRINT_STRING, "File not found", 0, 0);
	}

	// write <fileName>
	// Creates a long prompt to write to a new file titled fileName
	else if (tryCommand("write", input)){
		commandLength = 6;
		bufferRoot = buffer;

		while (1) {  // write lines to buffer
			syscall(PRINT_STRING, ":", 0, 0);
			syscall(READ_STRING, line, 0, 0);
			if (line[0] == '\r') break;  // break case on empty line

			i = 0;
			while (line[i]){  // copy line to buffer
				buffer[0] = line[i];
				buffer++; i++;
			}
		}

		buffer = bufferRoot;  // write buffer to file
		syscall(WRITE_FILE, buffer, input + commandLength, 1);
	}

	// kill <pid>
	// Kills the process with id <pid>
	else if (tryCommand("kill", input)){
		commandLength = 5;
		i = *(input + commandLength) - '0';
		if (i > 0 && i < 8) syscall(KILL, i, 0, 0);
		else syscall(PRINT_STRING, "Invalid pid", 0, 0);
	}

	else syscall(PRINT_STRING, "Command not found", 0, 0);

	syscall(PRINT_STRING, "\r\n", 0, 0);

}

// checks the input against the command
int tryCommand(char* command, char* input){

	int i = 0;
	while (command[i] != 0){
		if (command[i] != input[i]) return 0;
		i++;
	}
	return 1;
}
