CFLAGS = -g -Wall -pedantic $(CFLAGS)
parser: parser.c
	$(CC) -o parser parser.c
