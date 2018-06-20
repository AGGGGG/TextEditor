CC = gcc

CCFLAGS = -Wall -g -std=c99

BINARIES = editor

editor: editor.o
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	/bin/rm -f *.o $(BINARIES)
