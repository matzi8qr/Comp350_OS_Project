Tyler Wormald, Sean Howe, Timothy Morel

Compile with: $ ./compileOS.sh<br>
Creates a disk image "diskc.img" with a bootloader, kernel, shell, and small directory.\n
(can be run with mdblack/simulator)

Commands:
$ dir\n
Prints out the directory
	
$ type {fileName}\n
Prints the contents of file fileName to screen

$ exec {fileName}\n
Executes the contents of file fileName
	
$ del {fileName}\n
Deletes file fileName
	
$ copy {targetFile} {destinationFile}\n
Copies contents of targetFile to destinationFile
	
$ write {fileName}\n
Initiates a prompt to create and write to a file fileName
	
$ kill {pid}\n
For threading, kills the running process with id pid (between 1 and 7 inclusively, process 0 is shell)
