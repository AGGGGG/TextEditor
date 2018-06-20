// editor.c: turns terminal into a text editor 
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

void enable_raw_mode()
{
	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);
	raw.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main()
{
	enable_raw_mode();

	char c;
	//now, read in until either 1) there's an error or 2) a q is typed
	while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
	return 0;
}