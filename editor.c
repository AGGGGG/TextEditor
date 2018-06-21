// editor.c: turns terminal into a text editor 

/************ included libraries **************/
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h> //for ioctl
#include <ctype.h> //like cctype (for character operations)
#include <stdio.h> //for printf
#include <string.h> //for memcpy


/************ DEFINES **************/
#define CTRL_KEY(k) ((k) & 0x1f) // NOTE: this doesn't work; fix this and look into bit ops


/************ global initialized data **************/
//make this variable global so can use anywhere
//because not sure how to pass paramters to fucntion pointer
//when calling atexit
struct Terminal_Settings
{
	int rows;
	int cols; // these last two variables dictate screen size
	struct termios orig_settings;
};

struct Terminal_Settings original;

struct abuf
{
	char *b;
	int length;
};

struct abuf ab;


/************* ABUF FUNCTIONS **************/
//basically a constructor
void init_abuf(struct abuf* ab)
{
	ab->b = "";
	ab->length = 0;
}

void abAppend(struct abuf *ab, char *str, int str_length)
{
	if(str_length == 0) return;

	char *new_str = realloc(ab, ab->length + str_length);
	memcpy(&new_str[ab->length], str, str_length);
	ab->b = new_str;
	ab->length += str_length;

}

void abFree(struct abuf *ab)
{
	free(ab->b);
}



/************ TERMINAL SETTINGS AND UTIL FUNCTIONS **************/
//handle errors
void handle_error(const char* error_loc)
{
	//clear_screen();

	perror(error_loc); //prints error message (with value in errno)
	exit(1);
}
//restore terminal to original settings by saving the original values
//and setting it to them when the program exits
void disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &original.orig_settings);
}
void enable_raw_mode()
{
	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);
	original.orig_settings = raw; //save original settings

	//now, format raw's flags and characteristics to be what we want
	raw.c_lflag &= ~(ECHO); //so you can't see what you type
	raw.c_lflag &= ~(ICANON); //so the progam read input byte-by-byte (not when user presses enter)
	//raw.c_lflag &= ~(ISIG); //so that ctrl-c and ctrl-z don't send signals anymore
	raw.c_lflag &= ~(IXON); //so ctrl-s and ctrl-Q don't send signals anymore
	raw.c_lflag &= ~(IEXTEN); //disable ctrl-V
	raw.c_lflag &= ~(ICRNL); //so 13 (\r) isn't translated to 10 (a newline)
	raw.c_lflag &= ~(OPOST); //turn off default output processing
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

	atexit(disable_raw_mode); //make sure that original settings restored when program exits
}

char read_keypress()
{
	//printf("in readkeypress\n"); //DEBUGGING
	char c = '\0';
	int nread;
	while((nread = read(STDIN_FILENO, &c, 1)) == 1)
	{
		if(nread == -1) handle_error("read");
	}
	//printf("leaving read keypress"); //DEBUGGING
	return c;
}

int get_cursor_pos(int *rows, int *cols)
{
	//if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
	abAppend(&ab, "\x1b[6n", 4);
	
	//now, read what the terminal has printed to us to get cursor pos
	char buffer[50]; //size should provide more than enough characters
	char c;
	int index = 0;
	while(read(STDIN_FILENO, &c, 1) == 1)
	{
		if(c == 'R') break;
		buffer[index] = c;
		index++;
	}

	//now, we have a buffer of the form \x1b[#;#
	sscanf(&buffer[2], "%d;%d", rows, cols);
}

int get_window_size(int *rows, int *cols)
{
	struct winsize ws; //window size struct
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
	{
		//if(write(STDOUT_FILENO, "\x1b[999C;\x1b[999B", 12) == -1) return -1;
		abAppend(&ab, "\x1b[999C;\x1b[999B", 12);
		return get_cursor_pos(rows, cols);

	}
	else
	{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}


/************ FORMAT OUTPUT**************/
void clear_screen()
{
	init_abuf(&ab);
	//write(STDOUT_FILENO, "\x1b[2J", 4);
	abAppend(&ab, "\x1b[2J", 4);
	abAppend(&ab, "\x1b[H", 4);
	//write(STDOUT_FILENO, "\x1b[H", 3);

	write(STDOUT_FILENO, ab.b, ab.length);
	abFree(&ab);
}

void draw_rows()
{
	for(int row = 0; row < original.rows; row++)
	{
		//write(STDOUT_FILENO, "$", 1);
		abAppend(&ab, "$", 1);

		if(row < original.rows-1)
		{
			//write(STDOUT_FILENO,"\n", 1);
			abAppend(&ab, "\n", 1);
		}
	}

	//return cursor to original spot
	//write(STDOUT_FILENO, "\x1b[H", 3);
	abAppend(&ab, "\x1b[H", 3);
}

void init_screen()
{
	clear_screen();
	enable_raw_mode();
	get_window_size(&original.rows, &original.cols); //set screen size in struct
	draw_rows();

}


/************ FORMAT INPUT **************/
void process_keypress()
{
	//printf("in processkeypress"); //DEBUGGING
	char c = read_keypress();

	//printf("before switch"); //DEBUGGING
	switch(c)
	{
		case ('q'):
			//printf("case q"); //DEBUGGING
			clear_screen();
			exit(0);
			break;
		default:
			printf("%d\n", c);
			break;
	}
}


/************ MAIN **************/
int main()
{
	init_screen();
//		printf("rows: %d, cols: %d", original.rows, original.cols);

	while(1)
	{
		process_keypress();
	}
	return 0;
}







