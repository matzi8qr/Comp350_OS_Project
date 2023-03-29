Tyler Wormald, Sean Howe, Timothy Morel

Compile with: $ ./compileOS.sh
Creates a disk image "diskc.img" with a bootloader, kernel, shell, and small directory.
(can be run with mdblack/simulator)

Commands:
$ dir
Prints out the directory
	
$ type <fileName>
Prints the contents of file fileName to screen

$ exec <fileName>
Executes the contents of file fileName
	
$ del <fileName>
Deletes file fileName
	
$ copy <targetFile> <destinationFile>
Copies contents of targetFile to destinationFile
	
$ write <fileName>
Initiates a prompt to create and write to a file fileName
	
$ kill <pid>
For threading, kills the running process with id pid (between 1 and 7 inclusively, process 0 is shell)
