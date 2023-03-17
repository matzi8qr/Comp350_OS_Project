// written by Tyler Wormald, Sean Howe, Timothy Morel
// initializes the kernel of the OS

void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*, int);
void handleInterrupt(int, char*, char*, int);
void readFile(char*, char*, int*);
void executeProgram(char*);
void writeSector(char*, int);
void terminate();
void deleteFile(char*, int*);
void writeFile(char* char* int);
void handleTimerInterrupt(int, int);
void kill(int);
int mod(int, int);

int processActive[8];
int processStackPointer[8];
int currentProcess;

int main(){

	int i;
	makeInterrupt21();

	for (i = 0; i < 8; i++){
		processActive[i] = 0;
		processStackPointer[i] = 0xff00;
	}
	currentProcess = -1;

	executeProgram("shell");
	makeTimerInterrupt();
	while (1);
}

// Calls printChar for interrupt10, far more readable
void printChar(char c){

	interrupt(0x10, 0xe*0x100 + c, 0, 0, 0);
}

// Takes a string and prints it to screen by calling printChar interrupt
void printString(char* str){

	char c = '0';
	int i = 0;
	while(1){
		c = str[i];
		if (!c)	return;  // break case: if c is null
		printChar(c);
		i++;
	}
}


// Takes empty (or not!) array and fills it with what is typed
void readString(char* str){

	int i = 0;
	char in;
	while(1) {
		in = interrupt(0x16, 0, 0, 0, 0);  // interrupt16 is keyboard read, 1 char at a time
		if (in == 0xd) {  // Break case on in=ENTER
			str[i] = '\r';
			str[i+1] = '\n';
			str[i+2] = 0;
			printChar('\r');
			printChar('\n');
			return;
		}
		if (in == 0x8) {  // Backspace case
			if (i == 0) continue;  // don't backspace empty string!
			printChar(in);
			printChar(0x20);
			printChar(in);
			i--;
			str[i] = 0;
		}
		else {
			printChar(in);
			str[i] = in;
			i++;
		}
	}
}

// idk man it reads a disk sector with interrupt13
void readSector(char* buffer, int sector){

	interrupt(0x13, 0x2 * 0x100 + 0x1, buffer, sector + 1, 0x80);
}

// reads a file with fileName from directory, finds it, then loads the file into buffer
void readFile(char* fileName, char* buffer, int* sectors){

	int fileEntry = 0;
	char dir[512];
	int sectorPointer = 0;
	*sectors = 0;

	// search directory for filename
	readSector(dir, 2);  // load Directory sector into dir
	for (fileEntry = 0; fileEntry < 0x200; fileEntry += 32){  // for each file in directory
		if (fileName[0] != dir[fileEntry + 0]) continue;  // these 6 cases will skip to next iteration if
		if (fileName[1] != dir[fileEntry + 1]) continue;  // the name doesn't match
		if (fileName[2] != dir[fileEntry + 2]) continue;
		if (fileName[3] != dir[fileEntry + 3]) continue;
		if (fileName[4] != dir[fileEntry + 4]) continue;
		if (fileName[5] != dir[fileEntry + 5]) continue;
		sectorPointer = fileEntry + 6;
		break;
	}

	if (!sectorPointer) return;  // if nothing is found, don't populate sectors and get out

	// start actually reading the file
	while(dir[sectorPointer]){ // loads the file into buffer
		readSector(buffer, dir[sectorPointer]);
		buffer += 512;
		sectorPointer++;
		(*sectors)++;
	}
}

// allows our OS to execute programs, yknow, being useful
void executeProgram(char* name){

	char buffer[13312];
	int address = 0;
	int sectors;
	int segment;
	int i = 0;
	int dataseg;

	readFile(name, buffer, &sectors);
	if (!sectors) return;

	dataseg = setKernelDataSegment();

	// find free segment
	for (i = 0; i < 8; i++){
		if (processActive[i]) continue;
		processStackPointer[i] = 0xff00;
		segment = (i + 2) * 0x1000;
		break;
	}
	restoreDataSegment(dataseg);

	// load file to memory
	for (address = 0; address < 13312; address++){
		putInMemory(segment, address, buffer[address]);
	}

	// houston we have liftoff
	dataseg = setKernelDataSegment();
	initializeProgram(segment);
	processActive[i] = 1;
	restoreDataSegment(dataseg);
}

// EXTERMINATE! EXTERMINATE! EXTERMINATE!
void terminate(){

	setKernelDataSegment();
	processActive[currentProcess] = 0;
	while (1);
}

// calls interrupt 0x13 to write to a sector, yay
void writeSector(char* buffer, int sector){

	interrupt(0x13, 0x3 * 0x100 + 0x1, buffer, sector + 1, 0x80);
}

// allows file fileName to be overwritten, effectively deleting it.
void deleteFile(char* fileName, int *sectors){

	char dir[512];
	char map[512];
	int fileEntry;
	*sectors = 0;

	readSector(dir, 2);
	readSector(map, 1);
	for (fileEntry = 0; fileEntry < 0x200; fileEntry += 32){  // for each file in directory
		if (fileName[0] != dir[fileEntry + 0]) continue;  // these 6 cases will skip to next iteration if
		if (fileName[1] != dir[fileEntry + 1]) continue;  // the name doesn't match
		if (fileName[2] != dir[fileEntry + 2]) continue;
		if (fileName[3] != dir[fileEntry + 3]) continue;
		if (fileName[4] != dir[fileEntry + 4]) continue;
		if (fileName[5] != dir[fileEntry + 5]) continue;
		dir[fileEntry] = '\0';  // clears file out of dir
		*sectors = fileEntry + 6;
		break;
	}

	if (!(*sectors)) return;  // file not found

	while (dir[*sectors]){  // tell map that sectors are now free
		map[dir[*sectors]] = 0;
		(*sectors)++;
	}

	writeSector(dir, 2);
	writeSector(map, 1);
}

// lets us write files to the disk
void writeFile(char* buffer, char* fileName, int sectors){

	char map[512];
	char dir[512];
	int dirIndex;
	int i;
	int mapIndex;

	readSector(map, 1);
	readSector(dir, 2);

	// Find a free directory entry
	for (dirIndex = 0; dirIndex < 512; dirIndex += 32) {
		if (dir[dirIndex]) continue;  // space taken
		break;
	}

	if (dirIndex == 512) return;  // no room :(

	i = dirIndex;  // next line copies fileName into dir
	for (dirIndex = i; dirIndex < i + 6; dirIndex++) dir[dirIndex] = fileName[dirIndex - i];

	for (mapIndex = 3; mapIndex < 512; mapIndex++) {  // write buffer to free sectors
		if (map[mapIndex]) continue;  // space taken
		map[mapIndex] = 0xff;
		dir[dirIndex] = mapIndex;
		writeSector(buffer, mapIndex);
		buffer += 512;
		dirIndex++;
		if (!(--sectors)) break;  // no more sectors specified? good. file is in, now leave.
	}

	writeSector(map, 1);
	writeSector(dir, 2);  // commit changes. good job.
}

// The Family sends their regards.
void kill(int pid){

	int dataseg = setKernelDataSegment();
	processActive[pid] = 0;
	restoreDataSegment(dataseg);
}


// turns our methods into interrupt commands
void handleInterrupt21(int ax, char* bx, char* cx, int* dx){

	if (ax == 0) printString(bx);
	else if (ax == 1) readString(bx);
	else if (ax == 2) readSector(bx, cx);
	else if (ax == 3) readFile(bx, cx, dx);
	else if (ax == 4) executeProgram(bx);
	else if (ax == 5) terminate();
	else if (ax == 6) writeSector(bx, cx);
	else if (ax == 7) deleteFile(bx, cx);
	else if (ax == 8) writeFile(bx, cx, dx);
	else if (ax == 9) kill(bx);
	else {
		char* str = "interrupt 0x21 command not found";
		printString(str);

	}
}

// it... handles... the timer... interrupt...
void handleTimerInterrupt(int segment, int sp){

	int i;
	int nextProcess = currentProcess;
	int dataseg = setKernelDataSegment();

	if (currentProcess != -1) processStackPointer[currentProcess] = sp;
	for (i = 1; i <= 8; i++) {  // find an active process to run
		nextProcess = mod(currentProcess + i, 8);
		if (!processActive[nextProcess]) continue;
		currentProcess = nextProcess;
		break;
	}
	segment = (currentProcess + 2) * 0x1000;
	sp = processStackPointer[currentProcess];
	restoreDataSegment(dataseg);
	returnFromTimer(segment, sp);
}

// basic modulo function for positive numbers
int mod(int operand, int modulus){

	if (operand < modulus) return operand;
	return mod(operand - modulus, modulus);
}
