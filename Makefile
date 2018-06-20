CC = cc

CCFLAGS = -Wall -g -std=c99


editor: editor.o
	$(CC) $(CCFLAGS) -o $@ $^
