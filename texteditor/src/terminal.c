#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

void die(const char *s) {
  perror(s);
  exit(1);
}

struct termios terminalEnableRawMode(){
	// Holds the current attributes of the terminal
	struct termios attribs;
	struct termios original_attribs;

	// Read in the attributes of the terminal
	if(tcgetattr(STDIN_FILENO, &attribs) == -1) die("tcgetattr");
	original_attribs = attribs;

	// Change terminal attributs
	attribs.c_lflag &= ~(ECHO); // Dont write users input to screen
	attribs.c_lflag &= ~(ICANON); // Dont wait for enter to read input, read byte by byte

	// Dont exit process on signal interupt (CTR + C or Z)
	// As well as CTR + V (IEXTEN)
	attribs.c_lflag &= ~(ISIG | IEXTEN); 

	// Disable weird old thing where you could disable transmittion is CTR + S and enable with CTR + Q
	// Disable automatic conversion of carriage returns (13, '\r') inputted by the user into newlines (10, '\n'): ICRNL
	attribs.c_iflag &= ~(IXON | ICRNL);

	// Disbales automatic translation of each newline ("\n") we print into a carriage return followed by a newline ("\r\n")
	attribs.c_oflag &= ~(OPOST);

	// VMIN sets the minimum number of bytes of input untill 'read' returns 
	// VTIME value sets the maximum amount of time to wait before read() returns
	// it is in tenths of a second, so we set it to 1/10 of a second, or 100 milliseconds
	attribs.c_cc[VMIN] = 0;
	attribs.c_cc[VTIME] = 1;

	// Set changed attributs to current terminal 
	// TCSAFLUSH specifies when to apply these changes
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &attribs) == -1) die("tcsetattr");

	return original_attribs;
}

void terminalDisableRawMode(struct termios* original){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, original) == -1) die("tcsetattr");
}


int terminalGetCursorPos(int* row, int* col) {
	unsigned int buf_size = 32;
	char buf[buf_size];
	unsigned int buf_index = 0;

	// Request cursor position (reports as ESC[#;#R) which we can parse
	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

	// Read in cursor position string from terminal
	while (buf_index < buf_size - 1) {
	if (read(STDIN_FILENO, &buf[buf_index], 1) != 1) break;
	buf_index++;
	}
	buf[buf_index] = '\0';

	// Parse row and column from buffer
	if (buf[0] != '\x1b' || buf[1] != '[') return -1;
	if (sscanf(&buf[2], "%d;%d", row, col) != 2) return -1;

	return 1;
}

int terminalGetTermDim(int* rows, int* cols) {
  struct winsize winsize;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1 || winsize.ws_col == 0) {

	// Could not get terminal size from ioctl
	// Parse ANSI Escape Sequences instaed
	// First move cursor to the bottom right of screen 
	// This assumes terminal is less than 999x999
	// Then check the postion of the cursor to get terminal size
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    if (terminalGetCursorPos(rows, cols) == -1) return -1;
  } else {
    *cols = winsize.ws_col;
    *rows = winsize.ws_row;
  }

  return 0;
}

// Monitor STDIN file descriptor to check if its read for I/O
// timeout of 0s so it wont block if the fd is not ready
// If STDIN is read to read from it returns a non-zero value
// else it returns 0
int terminalIsKeyHit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
