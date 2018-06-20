// editor.c: turns terminal into a text editor 
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h> //like cctype (for character operations)
#include <stdio.h> //for printf

//make this variable global so can use anywhere
//because not sure how to pass paramters to fucntion pointer
//when calling atexit
struct termios orig_settings;


//restore terminal to original settings by saving the original values
//and setting it to them when the program exits
void disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_settings);
}
void enable_raw_mode()
{
	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);
	orig_settings = raw; //save original settings

	raw.c_lflag &= ~(ECHO); //so you can't see what you type
	raw.c_lflag &= ~(ICANON); //so the progam read input byte-by-byte (not when user presses enter)
	raw.c_lflag &= ~(ISIG); //so that ctrl-c and ctrl-z (and any other ctrls) don't send signals anymore
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

	atexit(disable_raw_mode); //make sure that original settings restored when program exits
}



int main()
{
	enable_raw_mode();

	char c;
	//now, read in until either 1) there's an error or 2) a q is typed
	//in the loop, print out characters
	while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
	{
		if(iscntrl(c))
		{
			//not a readable character
			printf("%d\n", c);
		} 
		else
		{
			printf("%d ('%c')\n", c, c);
		}
	}
	return 0;
}







