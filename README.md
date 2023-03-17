Tyler Wormald, Sean Howe, Timothy Morel

diskc.img contains an operating system with, currently, three very simple abilities:
	interrupt 0x21 AX = 0: prints a string with address BX to the screen
	interrupt 0x21 AX = 1: reads what is typed to the shell until the ENTER key is pressed, saving it to
				address BX
	interrupt 0x21 AX = 2: reads a sector number CX and copies to address BX

Currently, OS simply echoes whatever is typed to the shell once and only once. so overall, not very useful.

To run, load diskc.img into the simulator with cylinders=63, heads=255, sectors=1000. If it workss, you should see
a ':' print to screen as a prompt. Start typing to watch the magic happen.
