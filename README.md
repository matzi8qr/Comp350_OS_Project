Tyler Wormald, Sean Howe, Timothy Morel

Compile with: $ ./compileOS.sh<br>
Creates a disk image "diskc.img" with a bootloader, kernel, shell, and small directory.<br>
(can be run with mdblack/simulator)

Commands:<br>
$ dir<br>
Prints out the directory
	
$ type {fileName}<br>
Prints the contents of file fileName to screen

$ exec {fileName}<br>
Executes the contents of file fileName
	
$ del {fileName}<br>
Deletes file fileName
	
$ copy {targetFile} {destinationFile}<br>
Copies contents of targetFile to destinationFile
	
$ write {fileName}<br>
Initiates a prompt to create and write to a file fileName
	
$ kill {pid}<br>
For threading, kills the running process with id pid (between 1 and 7 inclusively, process 0 is shell)
