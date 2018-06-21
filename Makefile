CC = gcc

CCFLAGS = -Wall -g -std=c99

BINARIES = editor

editor: editor.o
	$(CC) $(CCFLAGS) -o $@ $^

editor.o: editor.c
	$(CC) $(CCFLAGS) -c $^

clean:
	/bin/rm -f *.o $(BINARIES)
